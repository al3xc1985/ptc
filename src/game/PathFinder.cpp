/*
 * Copyright (C) 2005-2011 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "MoveMap.h"
#include "Map.h"
#include "Creature.h"
#include "PathFinder.h"
#include "Log.h"

#include "DetourCommon.h"

////////////////// PathInfo //////////////////
PathInfo::PathInfo(const Unit* owner, float destX, float destY, float destZ, bool forceDest) :
    m_polyLength(0), m_type(PATHFIND_BLANK),
    m_forceDestination(forceDest), m_useStraightPath(true), m_pointPathLimit(MAX_POINT_PATH_LENGTH),
    m_sourceUnit(owner), m_navMesh(NULL), m_navMeshQuery(NULL)
{
    PathNode endPoint(destX, destY, destZ);
    setEndPosition(endPoint);

    float x,y,z;
    m_sourceUnit->GetPosition(x, y, z);
    PathNode startPoint(x, y, z);
    setStartPosition(startPoint);

    //DEBUG_FILTER_LOG(LOG_FILTER_PATHFINDING, "++ PathInfo::PathInfo for %u \n", m_sourceUnit->GetGUIDLow());

    uint32 mapId = m_sourceUnit->GetMapId();
    if (MMAP::MMapFactory::IsPathfindingEnabled(mapId))
    {
        MMAP::MMapManager* mmap = MMAP::MMapFactory::createOrGetMMapManager();
        m_navMesh = mmap->GetNavMesh(mapId);
        m_navMeshQuery = mmap->GetNavMeshQuery(mapId, m_sourceUnit->GetInstanceId());
    }

    createFilter();

    if (m_navMesh && m_navMeshQuery && HaveTile(endPoint) &&
            !m_sourceUnit->hasUnitState(UNIT_STAT_IGNORE_PATHFINDING))
    {
        BuildPolyPath(startPoint, endPoint);
    }
    else
    {
        BuildShortcut();
        m_type = PathType(PATHFIND_NORMAL | PATHFIND_NOT_USING_PATH);
    }
}

PathInfo::~PathInfo()
{
    //DEBUG_FILTER_LOG(LOG_FILTER_PATHFINDING, "++ PathInfo::~PathInfo() for %u \n", m_sourceUnit->GetGUIDLow());
}

bool PathInfo::Update(float destX, float destY, float destZ, bool forceDest)
{
    float x, y, z;
    m_sourceUnit->GetPosition(x, y, z);

    if (!Oregon::IsValidMapCoord(destX, destY, destZ) || !Oregon::IsValidMapCoord(x, y, z))
    {
        sLog.outDebug("PathInfo::Update() called with invalid map coords, destX: %f destY: %f destZ: %f x: %f y: %f z: %f for creature %u", destX, destY, destZ, x, y, z, m_sourceUnit->GetGUIDLow());
        m_type = PATHFIND_NOPATH;
        return false;
    }

    PathNode oldDest = getEndPosition();
    PathNode newDest(destX, destY, destZ);
    setEndPosition(newDest);

    PathNode newStart(x, y, z);
    PathNode oldStart = getStartPosition();
    setStartPosition(newStart);

    m_forceDestination = forceDest;

    sLog.outDebug("PathInfo::Update() for %u \n", m_sourceUnit->GetGUIDLow());

    // make sure navMesh works - we can run on map w/o mmap
    // check if the start and end point have a .mmtile loaded (can we pass via not loaded tile on the way?)
    if (!m_navMesh || !m_navMeshQuery || m_sourceUnit->hasUnitState(UNIT_STAT_IGNORE_PATHFINDING) ||
        !HaveTile(newStart) || !HaveTile(newDest))
    {
        BuildShortcut();
        m_type = PathType(PATHFIND_NORMAL | PATHFIND_NOT_USING_PATH);
        return true;
    }

    updateFilter();

    // check if destination moved - if not we can optimize something here
    // we are following old, precalculated path?
    float dist = m_sourceUnit->GetObjectSize();

    if (inRange(oldDest, newDest, dist, dist) && m_pathPoints.size() > 2)
    {
        // our target is not moving - we just coming closer
        // we are moving on precalculated path - enjoy the ride
        //DEBUG_FILTER_LOG(LOG_FILTER_PATHFINDING, "++ PathInfo::Update:: precalculated path\n");

        m_pathPoints.crop(1, 0);
        setNextPosition(m_pathPoints[1]);
        return false;
    }
    // target moved, so we need to update the poly path
    BuildPolyPath(newStart, newDest);
    return true;
}

dtPolyRef PathInfo::getPathPolyByPosition(const dtPolyRef *polyPath, uint32 polyPathSize, const float* point, float *distance) const
{
    if (!polyPath || !polyPathSize)
        return INVALID_POLYREF;

    dtPolyRef nearestPoly = INVALID_POLYREF;
    float minDist2d = FLT_MAX;
    float minDist3d = 0.0f;

    for (uint32 i = 0; i < polyPathSize; ++i)
    {
        float closestPoint[VERTEX_SIZE];
        if (dtStatusFailed(m_navMeshQuery->closestPointOnPoly(polyPath[i], point, closestPoint, NULL)))
            continue;

        float d = dtVdist2DSqr(point, closestPoint);
        if (d < minDist2d)
        {
            minDist2d = d;
            nearestPoly = polyPath[i];
            minDist3d = dtVdistSqr(point, closestPoint);
        }

        if (minDist2d < 1.0f) // shortcut out - close enough for us
            break;
    }

    if (distance)
        *distance = dtSqrt(minDist3d);

    return (minDist2d < 3.0f) ? nearestPoly : INVALID_POLYREF;
}

dtPolyRef PathInfo::getPolyByLocation(const float* point, float *distance) const
{
    // first we check the current path
    // if the current path doesn't contain the current poly,
    // we need to use the expensive navMesh.findNearestPoly
    dtPolyRef polyRef = getPathPolyByPosition(m_pathPolyRefs, m_polyLength, point, distance);
    if(polyRef != INVALID_POLYREF)
        return polyRef;

    // we don't have it in our old path
    // try to get it by findNearestPoly()
    // first try with low search box
    float extents[VERTEX_SIZE] = {3.0f, 5.0f, 3.0f};    // bounds of poly search area
    float closestPoint[VERTEX_SIZE] = {0.0f, 0.0f, 0.0f};
    if (dtStatusSucceed(m_navMeshQuery->findNearestPoly(point, extents, &m_filter, &polyRef, closestPoint)) && polyRef != INVALID_POLYREF)
    {
        *distance = dtVdist(closestPoint, point);
        return polyRef;
    }

    // still nothing ..
    // try with bigger search box
    extents[1] = 200.0f;

    if (dtStatusSucceed(m_navMeshQuery->findNearestPoly(point, extents, &m_filter, &polyRef, closestPoint)) && polyRef != INVALID_POLYREF)
    {
        *distance = dtVdist(closestPoint, point);
        return polyRef;
    }

    return INVALID_POLYREF;
}

void PathInfo::BuildPolyPath(const PathNode &startPos, const PathNode &endPos)
{
    // *** getting start/end poly logic ***

    float distToStartPoly, distToEndPoly;
    float startPoint[VERTEX_SIZE] = {startPos.y, startPos.z, startPos.x};
    float endPoint[VERTEX_SIZE] = {endPos.y, endPos.z, endPos.x};

    dtPolyRef startPoly = getPolyByLocation(startPoint, &distToStartPoly);
    dtPolyRef endPoly = getPolyByLocation(endPoint, &distToEndPoly);

    // we have a hole in our mesh
    // make shortcut path and mark it as NOPATH ( with flying exception )
    // its up to caller how he will use this info
    if (startPoly == INVALID_POLYREF || endPoly == INVALID_POLYREF)
    {
        sLog.outDebug("BuildPolyPath :: (startPoly == 0 || endPoly == 0)\n");
        BuildShortcut();
        bool path = m_sourceUnit->GetTypeId() == TYPEID_UNIT && m_sourceUnit->ToCreature()->canFly();

        bool waterPath = m_sourceUnit->GetTypeId() == TYPEID_UNIT && m_sourceUnit->ToCreature()->canSwim();
        if (waterPath)
        {
            // Check both start and end points, if they're both in water, then we can *safely* let the creature move
            for (uint32 i = 0; i < m_pathPoints.size(); ++i)
            {
                ZLiquidStatus status = m_sourceUnit->GetBaseMap()->getLiquidStatus(m_pathPoints[i].x, m_pathPoints[i].y, m_pathPoints[i].z, MAP_ALL_LIQUIDS, NULL);
                // One of the points is not in the water, cancel movement.
                if (status == LIQUID_MAP_NO_WATER)
                {
                    waterPath = false;
                    break;
                }
            }
        }

        m_type = (path || waterPath) ? PathType(PATHFIND_NORMAL | PATHFIND_NOT_USING_PATH) : PATHFIND_NOPATH;
        return;
    }

    // we may need a better number here
    bool farFromPoly = (distToStartPoly > 7.0f || distToEndPoly > 7.0f);
    if (farFromPoly)
    {
        sLog.outDebug("BuildPolyPath :: farFromPoly distToStartPoly=%.3f distToEndPoly=%.3f\n", distToStartPoly, distToEndPoly);

        bool buildShotrcut = false;
        if (m_sourceUnit->GetTypeId() == TYPEID_UNIT)
        {
            Creature* owner = (Creature*)m_sourceUnit;

            PathNode p = (distToStartPoly > 7.0f) ? startPos : endPos;
            if (m_sourceUnit->GetMap()->IsUnderWater(p.x, p.y, p.z))
            {
                sLog.outDebug("BuildPolyPath :: underwater case\n");
                if (owner->canSwim())
                    buildShotrcut = true;
            }
            else
            {
                sLog.outDebug("BuildPolyPath :: flying case\n");
                if (owner->canFly())
                    buildShotrcut = true;
            }
        }

        if (buildShotrcut)
        {
            BuildShortcut();
            m_type = PathType(PATHFIND_NORMAL | PATHFIND_NOT_USING_PATH);
            return;
        }
        else
        {
            float closestPoint[VERTEX_SIZE];
            // we may want to use closestPointOnPolyBoundary instead
            if (dtStatusSucceed(m_navMeshQuery->closestPointOnPoly(endPoly, endPoint, closestPoint, NULL)))
            {
                dtVcopy(endPoint, closestPoint);
                setActualEndPosition(PathNode(endPoint[2],endPoint[0],endPoint[1]));
            }

            m_type = PATHFIND_INCOMPLETE;
        }
    }

    // *** poly path generating logic ***

    // start and end are on same polygon
    // just need to move in straight line
    if (startPoly == endPoly)
    {
        //DEBUG_FILTER_LOG(LOG_FILTER_PATHFINDING, "++ BuildPolyPath :: (startPoly == endPoly)\n");

        BuildShortcut();

        m_pathPolyRefs[0] = startPoly;
        m_polyLength = 1;

        m_type = farFromPoly ? PATHFIND_INCOMPLETE : PATHFIND_NORMAL;
        sLog.outDebug("BuildPolyPath :: path type %d\n", m_type);
        return;
    }

    // look for startPoly/endPoly in current path
    // TODO: we can merge it with getPathPolyByPosition() loop
    bool startPolyFound = false;
    bool endPolyFound = false;
    uint32 pathStartIndex = 0;
    uint32 pathEndIndex = 0;

    if (m_polyLength)
    {
        for (; pathStartIndex < m_polyLength; ++pathStartIndex)
        {
            // here to catch few bugs
            if (m_pathPolyRefs[pathStartIndex] == INVALID_POLYREF)
            {
                sLog.outDebug("Invalid poly ref in BuildPolyPath.");
                break;
            }
            
            if (m_pathPolyRefs[pathStartIndex] == startPoly)
            {
                startPolyFound = true;
                break;
            }
        }

        for (pathEndIndex = m_polyLength-1; pathEndIndex > pathStartIndex; --pathEndIndex)
            if (m_pathPolyRefs[pathEndIndex] == endPoly)
            {
                endPolyFound = true;
                break;
            }
    }

    if (startPolyFound && endPolyFound)
    {
        //DEBUG_FILTER_LOG(LOG_FILTER_PATHFINDING, "++ BuildPolyPath :: (startPolyFound && endPolyFound)\n");

        // we moved along the path and the target did not move out of our old poly-path
        // our path is a simple subpath case, we have all the data we need
        // just "cut" it out

        m_polyLength = pathEndIndex - pathStartIndex + 1;
        memmove(m_pathPolyRefs, m_pathPolyRefs+pathStartIndex, m_polyLength*sizeof(dtPolyRef));
    }
    else
    {
        //DEBUG_FILTER_LOG(LOG_FILTER_PATHFINDING, "++ BuildPolyPath :: (!startPolyFound && !endPolyFound)\n");

        // either we have no path at all -> first run
        // or something went really wrong -> we aren't moving along the path to the target
        // just generate new path

        // free and invalidate old path data
        clear();

        dtStatus dtResult = m_navMeshQuery->findPath(
                startPoly,          // start polygon
                endPoly,            // end polygon
                startPoint,         // start position
                endPoint,           // end position
                &m_filter,           // polygon search filter
                m_pathPolyRefs,     // [out] path
                (int*)&m_polyLength,
                MAX_PATH_LENGTH);   // max number of polygons in output path

        if (!m_polyLength || dtStatusFailed(dtResult))
        {
            // only happens if we passed bad data to findPath(), or navmesh is messed up
            sLog.outError("%u's Path Build failed: 0 length path", m_sourceUnit->GetGUIDLow());
            BuildShortcut();
            m_type = PATHFIND_NOPATH;
            return;
        }
    }

    // by now we know what type of path we can get
    if (m_pathPolyRefs[m_polyLength - 1] == endPoly && !(m_type & PATHFIND_INCOMPLETE))
        m_type = PATHFIND_NORMAL;
    else
        m_type = PATHFIND_INCOMPLETE;

    // generate the point-path out of our up-to-date poly-path
    BuildPointPath(startPoint, endPoint);
}

void PathInfo::BuildPointPath(const float *startPoint, const float *endPoint)
{
    float pathPoints[MAX_POINT_PATH_LENGTH*VERTEX_SIZE];
    uint32 pointCount = 0;
    dtStatus dtResult = DT_FAILURE;
    if (m_useStraightPath)
    {
        dtResult = m_navMeshQuery->findStraightPath(
                startPoint,         // start position
                endPoint,           // end position
                m_pathPolyRefs,     // current path
                m_polyLength,       // lenth of current path
                pathPoints,         // [out] path corner points
                NULL,               // [out] flags
                NULL,               // [out] shortened path
                (int*)&pointCount,
                m_pointPathLimit);   // maximum number of points/polygons to use
    }
    else
    {
        dtResult = findSmoothPath(
                startPoint,         // start position
                endPoint,           // end position
                m_pathPolyRefs,     // current path
                m_polyLength,       // length of current path
                pathPoints,         // [out] path corner points
                (int*)&pointCount,
                m_pointPathLimit);    // maximum number of points
    }

    if (pointCount < 2 || dtStatusFailed(dtResult))
    {
        // only happens if pass bad data to findStraightPath or navmesh is broken
        // single point paths can be generated here
        // TODO : check the exact cases
        sLog.outDebug("PathInfo::BuildPointPath FAILED! path sized %d returned\n", pointCount);
        BuildShortcut();
        m_type = PATHFIND_NOPATH;
        return;
    }
    else if (pointCount == m_pointPathLimit)
    {
        sLog.outDebug("PathInfo::BuildPointPath pointCount == m_pointPathLimit\n", pointCount);
        BuildShortcut();
        m_type = PATHFIND_SHORT;
        return;
    }

    m_pathPoints.resize(pointCount);
    for (uint32 i = 0; i < pointCount; ++i)
        m_pathPoints.set(i, PathNode(pathPoints[i*VERTEX_SIZE+2], pathPoints[i*VERTEX_SIZE], pathPoints[i*VERTEX_SIZE+1]));

    NormalizePath();

    // first point is always our current location - we need the next one
    setNextPosition(m_pathPoints[1]);
    setActualEndPosition(m_pathPoints[pointCount-1]);

    // force the given destination, if needed
    if(m_forceDestination &&
        (!(m_type & PATHFIND_NORMAL) || !inRange(getEndPosition(), getActualEndPosition(), 1.0f, 1.0f)))
    {
        // we may want to keep partial subpath
        if(dist3DSqr(getActualEndPosition(), getEndPosition()) < 
            0.3f * dist3DSqr(getStartPosition(), getEndPosition()))
        {
            setActualEndPosition(getEndPosition());
            m_pathPoints.set(m_pathPoints.size()-1, getEndPosition());
        }
        else
        {
            setActualEndPosition(getEndPosition());
            BuildShortcut();
        }

        m_type = PathType(PATHFIND_NORMAL | PATHFIND_NOT_USING_PATH);
    }

    //DEBUG_FILTER_LOG(LOG_FILTER_PATHFINDING, "++ PathInfo::BuildPointPath path type %d size %d poly-size %d\n", m_type, pointCount, m_polyLength);
}

void PathInfo::NormalizePath()
{
    for (uint32 i = 0; i < m_pathPoints.size(); ++i)
        m_sourceUnit->UpdateAllowedPositionZ(m_pathPoints[i].x, m_pathPoints[i].y, m_pathPoints[i].z);
}

void PathInfo::BuildShortcut()
{
    //DEBUG_FILTER_LOG(LOG_FILTER_PATHFINDING, "++ BuildShortcut :: making shortcut\n");

    clear();

    // make two point path, our curr pos is the start, and dest is the end
    m_pathPoints.resize(2);

    // set start and a default next position
    m_pathPoints.set(0, getStartPosition());
    m_pathPoints.set(1, getActualEndPosition());

    NormalizePath();

    setNextPosition(getActualEndPosition());
    m_type = PATHFIND_SHORTCUT;
}

void PathInfo::createFilter()
{
    uint16 includeFlags = 0;
    uint16 excludeFlags = 0;

    if (m_sourceUnit->GetTypeId() == TYPEID_UNIT)
    {
        Creature* creature = (Creature*)m_sourceUnit;
        if (creature->canWalk())
            includeFlags |= NAV_GROUND;          // walk

        // creatures don't take environmental damage
        if (creature->canSwim())
            includeFlags |= (NAV_WATER | NAV_MAGMA | NAV_SLIME);           // swim
    }
    else if (m_sourceUnit->GetTypeId() == TYPEID_PLAYER)
    {
        // perfect support not possible, just stay 'safe'
        includeFlags |= (NAV_GROUND | NAV_WATER);
    }

    m_filter.setIncludeFlags(includeFlags);
    m_filter.setExcludeFlags(excludeFlags);

    updateFilter();
}

void PathInfo::updateFilter()
{
    // allow creatures to cheat and use different movement types if they are moved
    // forcefully into terrain they can't normally move in
    if (m_sourceUnit->IsInWater() || m_sourceUnit->IsUnderWater())
    {
        uint16 includedFlags = m_filter.getIncludeFlags();
        includedFlags |= getNavTerrain(m_sourceUnit->GetPositionX(),
                                       m_sourceUnit->GetPositionY(),
                                       m_sourceUnit->GetPositionZ());

        m_filter.setIncludeFlags(includedFlags);
    }
}

NavTerrain PathInfo::getNavTerrain(float x, float y, float z)
{
    LiquidData data;
    ZLiquidStatus liquidStatus = m_sourceUnit->GetMap()->getLiquidStatus(x, y, z, MAP_ALL_LIQUIDS, &data);
    if (liquidStatus == LIQUID_MAP_NO_WATER)
        return NAV_GROUND;

    switch (data.type)
    {
        case MAP_LIQUID_TYPE_WATER:
        case MAP_LIQUID_TYPE_OCEAN:
            return NAV_WATER;
        case MAP_LIQUID_TYPE_MAGMA:
            return NAV_MAGMA;
        case MAP_LIQUID_TYPE_SLIME:
            return NAV_SLIME;
        default:
            return NAV_GROUND;
    }
}

bool PathInfo::HaveTile(const PathNode &p) const
{
    int tx = -1, ty = -1;
    float point[VERTEX_SIZE] = {p.y, p.z, p.x};

    m_navMesh->calcTileLoc(point, &tx, &ty);

    /// Workaround
    /// For some reason, often the tx and ty variables wont get a valid value
    /// Use this check to prevent getting negative tile coords and crashing on getTileAt
    if (tx < 0 || ty < 0)
        return false;

    return (m_navMesh->getTileAt(tx, ty, 0) != NULL);
}

uint32 PathInfo::fixupCorridor(dtPolyRef* path, uint32 npath, uint32 maxPath,
                               const dtPolyRef* visited, uint32 nvisited)
{
    int32 furthestPath = -1;
    int32 furthestVisited = -1;

    // Find furthest common polygon.
    for (int32 i = npath-1; i >= 0; --i)
    {
        bool found = false;
        for (int32 j = nvisited-1; j >= 0; --j)
        {
            if (path[i] == visited[j])
            {
                furthestPath = i;
                furthestVisited = j;
                found = true;
            }
        }
        if (found)
            break;
    }

    // If no intersection found just return current path.
    if (furthestPath == -1 || furthestVisited == -1)
        return npath;

    // Concatenate paths.

    // Adjust beginning of the buffer to include the visited.
    uint32 req = nvisited - furthestVisited;
    uint32 orig = uint32(furthestPath+1) < npath ? furthestPath+1 : npath;
    uint32 size = npath-orig > 0 ? npath-orig : 0;
    if (req+size > maxPath)
        size = maxPath-req;

    if (size)
        memmove(path+req, path+orig, size*sizeof(dtPolyRef));

    // Store visited
    for (uint32 i = 0; i < req; ++i)
        path[i] = visited[(nvisited-1)-i];

    return req+size;
}

bool PathInfo::getSteerTarget(const float* startPos, const float* endPos,
                              float minTargetDist, const dtPolyRef* path, uint32 pathSize,
                              float* steerPos, unsigned char& steerPosFlag, dtPolyRef& steerPosRef)
{
    // Find steer target.
    static const uint32 MAX_STEER_POINTS = 3;
    float steerPath[MAX_STEER_POINTS*VERTEX_SIZE];
    unsigned char steerPathFlags[MAX_STEER_POINTS];
    dtPolyRef steerPathPolys[MAX_STEER_POINTS];
    uint32 nsteerPath = 0;
    dtStatus dtResult = m_navMeshQuery->findStraightPath(startPos, endPos, path, pathSize,
                                                steerPath, steerPathFlags, steerPathPolys, (int*)&nsteerPath, MAX_STEER_POINTS);
    if (!nsteerPath || dtStatusFailed(dtResult))
        return false;

    // Find vertex far enough to steer to.
    uint32 ns = 0;
    while (ns < nsteerPath)
    {
        // Stop at Off-Mesh link or when point is further than slop away.
        if ((steerPathFlags[ns] & DT_STRAIGHTPATH_OFFMESH_CONNECTION) ||
            !inRangeYZX(&steerPath[ns*VERTEX_SIZE], startPos, minTargetDist, 1000.0f))
            break;
        ns++;
    }
    // Failed to find good point to steer to.
    if (ns >= nsteerPath)
        return false;

    dtVcopy(steerPos, &steerPath[ns*VERTEX_SIZE]);
    steerPos[1] = startPos[1];  // keep Z value
    steerPosFlag = steerPathFlags[ns];
    steerPosRef = steerPathPolys[ns];

    return true;
}

dtStatus PathInfo::findSmoothPath(const float* startPos, const float* endPos,
                                     const dtPolyRef* polyPath, uint32 polyPathSize,
                                     float* smoothPath, int* smoothPathSize, uint32 maxSmoothPathSize)
{
    *smoothPathSize = 0;
    uint32 nsmoothPath = 0;

    dtPolyRef polys[MAX_PATH_LENGTH];
    memcpy(polys, polyPath, sizeof(dtPolyRef)*polyPathSize);
    uint32 npolys = polyPathSize;

    float iterPos[VERTEX_SIZE], targetPos[VERTEX_SIZE];
    if (dtStatusFailed(m_navMeshQuery->closestPointOnPolyBoundary(polys[0], startPos, iterPos)))
        return DT_FAILURE;

    if (dtStatusFailed(m_navMeshQuery->closestPointOnPolyBoundary(polys[npolys-1], endPos, targetPos)))
        return DT_FAILURE;

    dtVcopy(&smoothPath[nsmoothPath*VERTEX_SIZE], iterPos);
    nsmoothPath++;

    // Move towards target a small advancement at a time until target reached or
    // when ran out of memory to store the path.
    while (npolys && nsmoothPath < maxSmoothPathSize)
    {
        // Find location to steer towards.
        float steerPos[VERTEX_SIZE];
        unsigned char steerPosFlag;
        dtPolyRef steerPosRef = INVALID_POLYREF;

        if (!getSteerTarget(iterPos, targetPos, SMOOTH_PATH_SLOP, polys, npolys, steerPos, steerPosFlag, steerPosRef))
            break;

        bool endOfPath = (steerPosFlag & DT_STRAIGHTPATH_END);
        bool offMeshConnection = (steerPosFlag & DT_STRAIGHTPATH_OFFMESH_CONNECTION);

        // Find movement delta.
        float delta[VERTEX_SIZE];
        dtVsub(delta, steerPos, iterPos);
        float len = dtSqrt(dtVdot(delta,delta));
        // If the steer target is end of path or off-mesh link, do not move past the location.
        if ((endOfPath || offMeshConnection) && len < SMOOTH_PATH_STEP_SIZE)
            len = 1.0f;
        else
            len = SMOOTH_PATH_STEP_SIZE / len;

        float moveTgt[VERTEX_SIZE];
        dtVmad(moveTgt, iterPos, delta, len);

        // Move
        float result[VERTEX_SIZE];
        const static uint32 MAX_VISIT_POLY = 16;
        dtPolyRef visited[MAX_VISIT_POLY];

        uint32 nvisited = 0;
        m_navMeshQuery->moveAlongSurface(polys[0], iterPos, moveTgt, &m_filter, result, visited, (int*)&nvisited, MAX_VISIT_POLY);
        npolys = fixupCorridor(polys, npolys, MAX_PATH_LENGTH, visited, nvisited);

        m_navMeshQuery->getPolyHeight(polys[0], result, &result[1]);
        result[1] += 0.5f;
        dtVcopy(iterPos, result);

        // Handle end of path and off-mesh links when close enough.
        if (endOfPath && inRangeYZX(iterPos, steerPos, SMOOTH_PATH_SLOP, 1.0f))
        {
            // Reached end of path.
            dtVcopy(iterPos, targetPos);
            if (nsmoothPath < maxSmoothPathSize)
            {
                dtVcopy(&smoothPath[nsmoothPath*VERTEX_SIZE], iterPos);
                nsmoothPath++;
            }
            break;
        }
        else if (offMeshConnection && inRangeYZX(iterPos, steerPos, SMOOTH_PATH_SLOP, 1.0f))
        {
            // Advance the path up to and over the off-mesh connection.
            dtPolyRef prevRef = INVALID_POLYREF;
            dtPolyRef polyRef = polys[0];
            uint32 npos = 0;
            while (npos < npolys && polyRef != steerPosRef)
            {
                prevRef = polyRef;
                polyRef = polys[npos];
                npos++;
            }

            for (uint32 i = npos; i < npolys; ++i)
                polys[i-npos] = polys[i];

            npolys -= npos;

            // Handle the connection.
            float startPos[VERTEX_SIZE], endPos[VERTEX_SIZE];
            if (dtStatusSucceed(m_navMesh->getOffMeshConnectionPolyEndPoints(prevRef, polyRef, startPos, endPos)))
            {
                if (nsmoothPath < maxSmoothPathSize)
                {
                    dtVcopy(&smoothPath[nsmoothPath*VERTEX_SIZE], startPos);
                    nsmoothPath++;
                }
                // Move position at the other side of the off-mesh link.
                dtVcopy(iterPos, endPos);
                m_navMeshQuery->getPolyHeight(polys[0], iterPos, &iterPos[1]);
                iterPos[1] += 0.5f;
            }
        }

        // Store results.
        if (nsmoothPath < maxSmoothPathSize)
        {
            dtVcopy(&smoothPath[nsmoothPath*VERTEX_SIZE], iterPos);
            nsmoothPath++;
        }
    }

    *smoothPathSize = nsmoothPath;

    // this is most likely a loop
    return nsmoothPath < MAX_POINT_PATH_LENGTH ? DT_SUCCESS : DT_FAILURE;
}

bool PathInfo::inRangeYZX(const float* v1, const float* v2, float r, float h) const
{
    const float dx = v2[0] - v1[0];
    const float dy = v2[1] - v1[1]; // elevation
    const float dz = v2[2] - v1[2];
    return (dx*dx + dz*dz) < r*r && fabsf(dy) < h;
}

bool PathInfo::inRange(const PathNode &p1, const PathNode &p2, float r, float h) const
{
    const float dx = p2.x - p1.x;
    const float dy = p2.y - p1.y;
    const float dz = p2.z - p1.z;
    return (dx*dx + dy*dy) < r*r && fabsf(dz) < h;
}

float PathInfo::dist3DSqr(const PathNode &p1, const PathNode &p2) const
{
    const float dx = p2.x - p1.x;
    const float dy = p2.y - p1.y;
    const float dz = p2.z - p1.z;
    return (dx*dx + dy*dy + dz*dz);
}
