-- Adds pathing for Lieutenant Valorcall and his guards
-- Path is between Stromgarde Keep, Arathi and Refuge Point, Arathi (and back)
-- NPCs are always spawned (unless killed) since there was no confirmation otherwise
-- 
-- NOTES:
--   * Valorcall is for Horde quest "Sigil of Arathor" (http://www.wowhead.com/quest=26032)
--   * It is proper that Valorcall will occasionally fight with the Forsaken Courier when their patrols cross.
--     Adjusted respawn allows for players to complete related quests.
--   * Could not confirm if NPC is only spawned when a player accepts the quest or is always spawned. Will require
--     c++ script if "quest accept" causes spawn / pathing.
--   * Could not confirm if NPC despawns once destination is reached or returns to spawn.

-- Lieutenant Valorcall
SET @npcLeaderId=14572;

-- Stromgarde Cavalryman
SET @npcFollowerId_1=14574;
SET @npcFollowerId_2=14575;
SET @wpPathId=(@npcLeaderId * 10);

-- Fix spawn info in Arathi Highlands
-- NOTE: Changed respawn to 1 minute because Valorcall is a quest NPC that can enter combat with (killed by) 
--       Forsaken Courier (another quest NPC) when their paths cross (which happens occasionally). Also, quest
--       research turned up comments that indicated these NPCs have a fast respawn.

DELETE FROM `creature` WHERE `guid` IN (@npcLeaderId,@npcFollowerId_1,@npcFollowerId_2);

INSERT INTO `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`) VALUES 
(@npcLeaderId,2612,0,1,4141,1,-1528.313843,-1843.296265,67.789253,1.530442,400,0,1,1135,1779,2),
(@npcFollowerId_1,2738,0,1,4145,1,-1525.899780,-1845.876465,67.764687,1.530442,400,0,0,958,0,0),
(@npcFollowerId_2,2738,0,1,4145,1,-1531.197144,-1845.848755,67.875923,1.530442,400,0,0,958,0,0);

-- Delete existing waypoints (if any)
DELETE FROM `waypoint_data` WHERE `id`=@wpPathId;

-- Add new waypoints
-- NOTE: It is unclear if the NPC should despawn when reaching the end of the path. For now, just have it pause
--       at end before starting the path back.

INSERT INTO `waypoint_data` (`id`, `point`, `position_x`, `position_y`, `position_z`, `orientation`, `delay`, `move_flag`, `action`, `action_chance`, `wpguid`) VALUES
(@wpPathId,1,-1528.31,-1843.30,67.79,1.530442,60000,0,0,100,0), -- 1 minute delay before starting
(@wpPathId,2,-1528.23,-1824.05,68.34,0,0,0,0,100,0),
(@wpPathId,3,-1528.32,-1820.46,68.9602,0,0,0,0,100,0),
(@wpPathId,4,-1528.37,-1818.28,69.5115,0,0,0,0,100,0),
(@wpPathId,5,-1528.64,-1808.76,71.3902,0,0,0,0,100,0),
(@wpPathId,6,-1528.77,-1800.37,71.2294,0,0,0,0,100,0),
(@wpPathId,7,-1528.82,-1795.04,70.1311,0,0,0,0,100,0),
(@wpPathId,8,-1528.84,-1793.37,69.5099,0,0,0,0,100,0),
(@wpPathId,9,-1528.87,-1790.61,68.9584,0,0,0,0,100,0),
(@wpPathId,10,-1528.82,-1785.05,68.3404,0,0,0,0,100,0),
(@wpPathId,11,-1527.51,-1762.21,67.0403,0,0,0,0,100,0),
(@wpPathId,12,-1529.35,-1746.33,66.8509,0,0,0,0,100,0),
(@wpPathId,13,-1529.5,-1732.3,66.2708,0,0,0,0,100,0),
(@wpPathId,14,-1539.93,-1714.01,66.363,0,0,0,0,100,0),
(@wpPathId,15,-1555.47,-1702.87,66.1763,0,0,0,0,100,0),
(@wpPathId,16,-1570.19,-1696.83,66.2239,0,0,0,0,100,0),
(@wpPathId,17,-1576.23,-1695.24,65.9776,0,0,0,0,100,0),
(@wpPathId,18,-1581.84,-1696.44,66.1376,0,0,0,0,100,0),
(@wpPathId,19,-1584.78,-1700.91,66.5301,0,0,0,0,100,0),
(@wpPathId,20,-1586.19,-1710.78,66.9607,0,0,0,0,100,0),
(@wpPathId,21,-1586.08,-1728.19,67.4567,0,0,0,0,100,0),
(@wpPathId,22,-1582.99,-1743.81,66.8545,0,0,0,0,100,0),
(@wpPathId,23,-1577.8,-1763.7,67.0782,0,0,0,0,100,0),
(@wpPathId,24,-1576.49,-1777.64,67.2815,0,0,0,0,100,0),
(@wpPathId,25,-1574.01,-1787.45,67.1901,0,0,0,0,100,0),
(@wpPathId,26,-1565.38,-1798.76,67.5678,0,0,0,0,100,0),
(@wpPathId,27,-1557.03,-1803.77,67.5158,0,0,0,0,100,0),
(@wpPathId,28,-1540,-1805.78,66.432,0,0,0,0,100,0),
(@wpPathId,29,-1513.43,-1805.73,66.1469,0,0,0,0,100,0),
(@wpPathId,30,-1490.95,-1805.66,68.0658,0,0,0,0,100,0),
(@wpPathId,31,-1466.56,-1803.87,67.7702,0,0,0,0,100,0),
(@wpPathId,32,-1452.87,-1800.94,65.4412,0,0,0,0,100,0),
(@wpPathId,33,-1438.86,-1799.91,61.945,0,0,0,0,100,0),
(@wpPathId,34,-1425.38,-1803.61,60.9587,0,0,0,0,100,0),
(@wpPathId,35,-1395.89,-1814.67,59.1394,0,0,0,0,100,0),
(@wpPathId,36,-1363.65,-1828.28,60.0694,0,0,0,0,100,0),
(@wpPathId,37,-1337.27,-1839.82,62.1984,0,0,0,0,100,0),
(@wpPathId,38,-1330.7,-1845.84,62.5435,0,0,0,0,100,0),
(@wpPathId,39,-1328.27,-1851.3,62.5249,0,0,0,0,100,0),
(@wpPathId,40,-1329.52,-1861.33,62.0214,0,0,0,0,100,0),
(@wpPathId,41,-1342.83,-1896.34,59.2707,0,0,0,0,100,0),
(@wpPathId,42,-1358.08,-1935.47,58.4276,0,0,0,0,100,0),
(@wpPathId,43,-1375.19,-1977.63,58.5289,0,0,0,0,100,0),
(@wpPathId,44,-1381.06,-2026.16,60.5442,0,0,0,0,100,0),
(@wpPathId,45,-1383.41,-2071.59,62.5124,0,0,0,0,100,0),
(@wpPathId,46,-1386.02,-2110,64.2494,0,0,0,0,100,0),
(@wpPathId,47,-1393.31,-2158.42,64.1111,0,0,0,0,100,0),
(@wpPathId,48,-1404.24,-2206.13,63.8535,0,0,0,0,100,0),
(@wpPathId,49,-1417.1,-2243.68,63.7514,0,0,0,0,100,0),
(@wpPathId,50,-1420.49,-2249.98,63.6904,0,0,0,0,100,0),
(@wpPathId,51,-1430.49,-2263.3,63.329,0,0,0,0,100,0),
(@wpPathId,52,-1441.13,-2277.2,62.7836,0,0,0,0,100,0),
(@wpPathId,53,-1446.84,-2287.53,61.9325,0,0,0,0,100,0),
(@wpPathId,54,-1449.19,-2299.55,61.8796,0,0,0,0,100,0),
(@wpPathId,55,-1449.73,-2327.53,61.7191,0,0,0,0,100,0),
(@wpPathId,56,-1450.03,-2355.53,61.5528,0,0,0,0,100,0),
(@wpPathId,57,-1450.15,-2380.03,61.4055,0,0,0,0,100,0),
(@wpPathId,58,-1450.49,-2397.53,61.0555,0,0,0,0,100,0),
(@wpPathId,59,-1453.92,-2410.52,60.03,0,0,0,0,100,0),
(@wpPathId,60,-1464.35,-2426.89,57.8205,0,0,0,0,100,0),
(@wpPathId,61,-1462.44,-2436.38,58.0342,0,0,0,0,100,0),
(@wpPathId,62,-1453.73,-2438.5,58.342,0,0,0,0,100,0),
(@wpPathId,63,-1440.72,-2440.2,58.9664,0,0,0,0,100,0),
(@wpPathId,64,-1424.83,-2453.5,59.0382,0,0,0,0,100,0),
(@wpPathId,65,-1412.62,-2462.65,57.2066,0,0,0,0,100,0),
(@wpPathId,66,-1396.91,-2462.16,57.0177,0,0,0,0,100,0),
(@wpPathId,67,-1380.78,-2455.37,55.0267,0,0,0,0,100,0),
(@wpPathId,68,-1364.89,-2450.51,52.0455,0,0,0,0,100,0),
(@wpPathId,69,-1351.11,-2448.04,49.9138,0,0,0,0,100,0),
(@wpPathId,70,-1336.2,-2448.68,46.4684,0,0,0,0,100,0),
(@wpPathId,71,-1319.9,-2455.01,43.1772,0,0,0,0,100,0),
(@wpPathId,72,-1304.21,-2462.75,39.1337,0,0,0,0,100,0),
(@wpPathId,73,-1289.01,-2471.38,33.4891,0,0,0,0,100,0),
(@wpPathId,74,-1275.46,-2482.79,26.7595,0,0,0,0,100,0),
(@wpPathId,75,-1267.01,-2493.63,22.4559,0,0,0,0,100,0),
(@wpPathId,76,-1254.69,-2512.44,20.5413,0,0,0,0,100,0),
(@wpPathId,77,-1250.21,-2521.37,20.4269,0,0,0,0,100,0),
(@wpPathId,78,-1246.19,-2529.78,20.6058,0,0,0,0,100,0),
(@wpPathId,79,-1242.05,-2535.15,21.0179,0,0,0,0,100,0),
(@wpPathId,80,-1242.99,-2526.84,20.9146,0,0,0,0,100,0),
(@wpPathId,81,-1250.83,-2520.96,20.426,0,60000,0,0,100,0), -- destination reached, 1 minute delay before pathing back
(@wpPathId,82,-1254.85,-2513.19,20.5112,0,0,0,0,100,0),
(@wpPathId,83,-1267.24,-2493.93,22.3988,0,0,0,0,100,0),
(@wpPathId,84,-1273.85,-2485.78,25.5707,0,0,0,0,100,0),
(@wpPathId,85,-1289.24,-2471.59,33.4718,0,0,0,0,100,0),
(@wpPathId,86,-1303.98,-2462.99,39.0339,0,0,0,0,100,0),
(@wpPathId,87,-1316.64,-2457,42.604,0,0,0,0,100,0),
(@wpPathId,88,-1335.67,-2449.4,46.2431,0,0,0,0,100,0),
(@wpPathId,89,-1349.51,-2448.47,49.503,0,0,0,0,100,0),
(@wpPathId,90,-1363.26,-2451.05,51.8257,0,0,0,0,100,0),
(@wpPathId,91,-1379.15,-2455.53,54.7104,0,0,0,0,100,0),
(@wpPathId,92,-1395.58,-2461.5,57.1067,0,0,0,0,100,0),
(@wpPathId,93,-1411.2,-2463.61,56.9774,0,0,0,0,100,0),
(@wpPathId,94,-1424.18,-2454.31,59.0152,0,0,0,0,100,0),
(@wpPathId,95,-1440.89,-2442.08,58.7537,0,0,0,0,100,0),
(@wpPathId,96,-1452.92,-2439.49,58.3243,0,0,0,0,100,0),
(@wpPathId,97,-1461.72,-2436.45,58.0823,0,0,0,0,100,0),
(@wpPathId,98,-1463.37,-2427.9,57.8549,0,0,0,0,100,0),
(@wpPathId,99,-1454.66,-2411.75,59.8753,0,0,0,0,100,0),
(@wpPathId,100,-1451.1,-2399.07,60.9648,0,0,0,0,100,0),
(@wpPathId,101,-1450.14,-2381.61,61.3911,0,0,0,0,100,0),
(@wpPathId,102,-1450.12,-2357.11,61.5426,0,0,0,0,100,0),
(@wpPathId,103,-1449.95,-2329.11,61.7066,0,0,0,0,100,0),
(@wpPathId,104,-1449.43,-2301.13,61.84,0,0,0,0,100,0),
(@wpPathId,105,-1446.58,-2287.66,61.9711,0,0,0,0,100,0),
(@wpPathId,106,-1442.44,-2279.13,62.5159,0,0,0,0,100,0),
(@wpPathId,107,-1431.93,-2265.15,63.3205,0,0,0,0,100,0),
(@wpPathId,108,-1421.36,-2251.22,63.6684,0,0,0,0,100,0),
(@wpPathId,109,-1418.07,-2245.04,63.7351,0,0,0,0,100,0),
(@wpPathId,110,-1405.35,-2208.74,63.8461,0,0,0,0,100,0),
(@wpPathId,111,-1394.01,-2161.12,64.1057,0,0,0,0,100,0),
(@wpPathId,112,-1386.65,-2112.7,64.4159,0,0,0,0,100,0),
(@wpPathId,113,-1383.82,-2074.31,62.6099,0,0,0,0,100,0),
(@wpPathId,114,-1381.54,-2028.88,60.6584,0,0,0,0,100,0),
(@wpPathId,115,-1375.91,-1979.41,58.5366,0,0,0,0,100,0),
(@wpPathId,116,-1358.95,-1937.19,58.4249,0,0,0,0,100,0),
(@wpPathId,117,-1343.72,-1898.05,59.126,0,0,0,0,100,0),
(@wpPathId,118,-1329.89,-1863.08,61.9069,0,0,0,0,100,0),
(@wpPathId,119,-1328.35,-1850.86,62.5391,0,0,0,0,100,0),
(@wpPathId,120,-1330.65,-1846.29,62.5257,0,0,0,0,100,0),
(@wpPathId,121,-1335.96,-1841.73,62.249,0,0,0,0,100,0),
(@wpPathId,122,-1361.59,-1829.16,60.2512,0,0,0,0,100,0),
(@wpPathId,123,-1393.84,-1815.58,59.0036,0,0,0,0,100,0),
(@wpPathId,124,-1423.47,-1804.92,60.7771,0,0,0,0,100,0),
(@wpPathId,125,-1438.32,-1800.12,61.8788,0,0,0,0,100,0),
(@wpPathId,126,-1450.61,-1800.69,64.7219,0,0,0,0,100,0),
(@wpPathId,127,-1464.33,-1803.5,67.6348,0,0,0,0,100,0),
(@wpPathId,128,-1488.18,-1805.87,68.074,0,0,0,0,100,0),
(@wpPathId,129,-1512.68,-1805.73,66.2104,0,0,0,0,100,0),
(@wpPathId,130,-1537.17,-1805.95,66.1776,0,0,0,0,100,0),
(@wpPathId,131,-1557.65,-1804.11,67.4839,0,0,0,0,100,0),
(@wpPathId,132,-1565.61,-1798.65,67.5617,0,0,0,0,100,0),
(@wpPathId,133,-1574.15,-1787.07,67.1787,0,0,0,0,100,0),
(@wpPathId,134,-1576.5,-1778.67,67.2749,0,0,0,0,100,0),
(@wpPathId,135,-1578.05,-1765.2,67.1097,0,0,0,0,100,0),
(@wpPathId,136,-1582.3,-1746.61,66.6746,0,0,0,0,100,0),
(@wpPathId,137,-1585.78,-1729.49,67.4793,0,0,0,0,100,0),
(@wpPathId,138,-1586.16,-1712.03,67.0329,0,0,0,0,100,0),
(@wpPathId,139,-1584.72,-1701.41,66.5389,0,0,0,0,100,0),
(@wpPathId,140,-1581.69,-1696.92,66.1604,0,0,0,0,100,0),
(@wpPathId,141,-1576.71,-1695.36,65.9892,0,0,0,0,100,0),
(@wpPathId,142,-1570.6,-1696.58,66.1985,0,0,0,0,100,0),
(@wpPathId,143,-1555.01,-1703.09,66.1586,0,0,0,0,100,0),
(@wpPathId,144,-1540.2,-1714.01,66.3447,0,0,0,0,100,0),
(@wpPathId,145,-1529.35,-1731.15,66.2182,0,0,0,0,100,0),
(@wpPathId,146,-1529.12,-1745.12,66.8033,0,0,0,0,100,0),
(@wpPathId,147,-1527.21,-1763.24,67.0823,0,0,0,0,100,0),
(@wpPathId,148,-1528.8,-1786.19,68.3406,0,0,0,0,100,0),
(@wpPathId,149,-1528.78,-1791.07,68.9602,0,0,0,0,100,0),
(@wpPathId,150,-1528.77,-1793.2,69.5115,0,0,0,0,100,0),
(@wpPathId,151,-1528.76,-1795.29,70.1611,0,0,0,0,100,0),
(@wpPathId,152,-1528.74,-1800.29,71.2228,0,0,0,0,100,0),
(@wpPathId,153,-1528.67,-1807.79,71.4654,0,0,0,0,100,0),
(@wpPathId,154,-1528.38,-1819.6,68.9567,0,0,0,0,100,0),
(@wpPathId,155,-1528.32,-1822.06,68.3404,0,0,0,0,100,0),
(@wpPathId,156,-1528.25,-1824.82,68.3404,0,0,0,0,100,0),
(@wpPathId,157,-1528.2,-1828.96,68.0658,0,0,0,0,100,0),
(@wpPathId,158,-1533.15,-1841.45,68.09,0,0,0,0,100,0),
(@wpPathId,159,-1528.19,-1853.83,67.62,0,0,0,0,100,0),
(@wpPathId,160,-1528.31,-1843.30,67.79,1.530442,0,0,0,100,0); -- ends at same coords as WP 1

-- Assign path and formation info
DELETE FROM `creature_formations` WHERE `leaderGUID`=@npcLeaderId;
INSERT INTO creature_formations  (`leaderGUID`,`memberGUID`,`dist`,`angle`,`groupAI`,`point_1`,`point_2`) VALUES 
(@npcLeaderId,@npcLeaderId,0,0,2,0,0),
(@npcLeaderId,@npcFollowerId_1,3,45,2,0,0), -- slightly behind and to the right
(@npcLeaderId,@npcFollowerId_2,3,315,2,0,0); -- slightly behind and to the left

DELETE FROM `creature_addon` WHERE `guid`=@npcLeaderId;
INSERT INTO `creature_addon` (`guid`, `path_id`, `mount`, `bytes1`, `bytes2`, `emote`, `auras`) VALUES
(@npcLeaderId,@wpPathId,229,0,4097,0,NULL);


-- Adds pathing for Forsaken Courier and her guards
-- Path is between GoShek Farm, Arathi and Tarren Mill, Hillsbrad (and back)
-- NPCs are always spawned (unless killed) since there was no confirmation otherwise
-- 
-- NOTES:
--   * Forsaken Courier is for Alliance quest "Hints of a New Plague" (http://www.wowhead.com/quest=658)
--   * It is proper that the Courier will occasionally fight with the Lieutenant Valorcall when their patrols cross.
--     Adjusted respawn allows for players to complete related quests.
--   * Could not confirm if NPC is only spawned when a player accepts the quest or is always spawned. Will require
--     c++ script if "quest accept" causes spawn / pathing.
--   * Could not confirm if NPC despawns once destination is reached or returns to spawn.

-- Forsaken Courier
SET @npcLeaderId=14772;

-- Forsaken Bodyguard
SET @npcFollowerId_1=14773;
SET @npcFollowerId_2=14774;
SET @npcFollowerId_3=14775;
SET @npcFollowerId_4=14776;
SET @wpPathId=(@npcLeaderId * 10);

-- Delete duplicate spawns in Hillsbrad
-- NOTE: The only records that should be present are `creature` but including the others just in case
DELETE FROM `creature` WHERE `guid` IN (16397,16402,16400,16399,16398);
DELETE FROM `creature_addon` WHERE `guid` IN (16397,16402,16400,16399,16398);
DELETE FROM `creature_formations` WHERE `leaderGUID` IN (16397,16402,16400,16399,16398);
DELETE FROM `waypoint_data` WHERE `id` IN (163970,164020,164000,163990,163980);

-- Fix spawn info in Arathi Highlands
-- NOTE: The patrol is supposed to spawn here when a particular quest is accepted, needs core script
-- NOTE: Changed respawn to 1 minute because the Courier is a quest NPC that can enter combat with (killed by) 
--       Lieutenant Valorcall (another quest NPC) when their paths cross (which happens occasionally). Also, quest
--       research turned up comments that indicated these NPCs have a fast respawn.

DELETE FROM `creature` WHERE `guid` IN (@npcLeaderId,@npcFollowerId_1,@npcFollowerId_2,@npcFollowerId_3,@npcFollowerId_4);

INSERT INTO `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`) VALUES 
(@npcLeaderId,2714,0,1,4041,1,-1552.426758,-3029.310791,13.644809,5.009628,60,0,1,1342,0,2),
(@npcFollowerId_1,2721,0,1,4037,1,-1551.740601,-3031.641846,13.644809,5.009628,60,0,0,610,0,0),
(@npcFollowerId_2,2721,0,1,4037,1,-1554.400024,-3029.450928,13.644809,5.009628,60,0,0,610,0,0),
(@npcFollowerId_3,2721,0,1,4038,1,-1553.325195,-3026.489502,13.644809,5.009628,60,0,0,610,0,0),
(@npcFollowerId_4,2721,0,1,4037,1,-1550.339844,-3028.255859,13.644809,5.009628,60,0,0,610,0,0);

-- Delete existing waypoints (if any)
DELETE FROM `waypoint_data` WHERE `id`=@wpPathId;

-- Add new waypoints
-- NOTE: It is unclear if the NPC should despawn when reaching the end of the path. For now, just have it pause
--       at end before starting the path back.

INSERT INTO `waypoint_data` (`id`, `point`, `position_x`, `position_y`, `position_z`, `orientation`, `delay`, `move_flag`, `action`, `action_chance`, `wpguid`) VALUES
(@wpPathId,1,-1552.43,-3029.31,13.6448,5.009628,60000,0,0,100,0), -- 1 minute delay before starting
(@wpPathId,2,-1551.28,-3042.15,12.9831,0,0,0,0,100,0),
(@wpPathId,3,-1551.47,-3055.87,13.0074,0,0,0,0,100,0),
(@wpPathId,4,-1557.96,-3065.89,13.2017,0,0,0,0,100,0),
(@wpPathId,5,-1568.38,-3066.59,13.1757,0,0,0,0,100,0),
(@wpPathId,6,-1584.38,-3059.72,12.8479,0,0,0,0,100,0),
(@wpPathId,7,-1604.79,-3043.72,14.9377,0,0,0,0,100,0),
(@wpPathId,8,-1633.49,-3013.09,19.2308,0,0,0,0,100,0),
(@wpPathId,9,-1640.02,-3004.89,20.9252,0,0,0,0,100,0),
(@wpPathId,10,-1645.71,-2989.32,23.6772,0,0,0,0,100,0),
(@wpPathId,11,-1645.99,-2971.86,26.3366,0,0,0,0,100,0),
(@wpPathId,12,-1643.19,-2959.2,27.7,0,0,0,0,100,0),
(@wpPathId,13,-1639.09,-2945.82,28.5385,0,0,0,0,100,0),
(@wpPathId,14,-1637.73,-2933.3,28.1553,0,0,0,0,100,0),
(@wpPathId,15,-1641.84,-2916.34,26.7999,0,0,0,0,100,0),
(@wpPathId,16,-1644.03,-2901.64,26.299,0,0,0,0,100,0),
(@wpPathId,17,-1639.03,-2885.94,27.6014,0,0,0,0,100,0),
(@wpPathId,18,-1631.71,-2866.27,30.2701,0,0,0,0,100,0),
(@wpPathId,19,-1625.44,-2842.61,32.2295,0,0,0,0,100,0),
(@wpPathId,20,-1621.03,-2822.09,33.6745,0,0,0,0,100,0),
(@wpPathId,21,-1617.7,-2804.91,34.6386,0,0,0,0,100,0),
(@wpPathId,22,-1612.97,-2777.32,36.1324,0,0,0,0,100,0),
(@wpPathId,23,-1610.07,-2756.52,37.2584,0,0,0,0,100,0),
(@wpPathId,24,-1606.39,-2739.47,37.6036,0,0,0,0,100,0),
(@wpPathId,25,-1598.18,-2724.09,37.1732,0,0,0,0,100,0),
(@wpPathId,26,-1586.96,-2710.69,36.1226,0,0,0,0,100,0),
(@wpPathId,27,-1572.97,-2695.7,34.6664,0,0,0,0,100,0),
(@wpPathId,28,-1564.09,-2685.41,35.4253,0,0,0,0,100,0),
(@wpPathId,29,-1557.93,-2669.06,36.761,0,0,0,0,100,0),
(@wpPathId,30,-1555.19,-2641.25,41.3576,0,0,0,0,100,0),
(@wpPathId,31,-1555.73,-2620.27,45.0843,0,0,0,0,100,0),
(@wpPathId,32,-1557.22,-2604.89,47.8118,0,0,0,0,100,0),
(@wpPathId,33,-1562.1,-2591.78,48.5331,0,0,0,0,100,0),
(@wpPathId,34,-1567.06,-2571.43,49.3074,0,0,0,0,100,0),
(@wpPathId,35,-1567.62,-2550.48,50.7276,0,0,0,0,100,0),
(@wpPathId,36,-1561.84,-2530.37,52.0972,0,0,0,0,100,0),
(@wpPathId,37,-1555.6,-2517.85,53.143,0,0,0,0,100,0),
(@wpPathId,38,-1550.4,-2506.83,53.9067,0,0,0,0,100,0),
(@wpPathId,39,-1544.74,-2495.89,54.115,0,0,0,0,100,0),
(@wpPathId,40,-1537.25,-2488.54,53.6039,0,0,0,0,100,0),
(@wpPathId,41,-1521.28,-2474.91,53.2915,0,0,0,0,100,0),
(@wpPathId,42,-1505.08,-2461.55,52.9902,0,0,0,0,100,0),
(@wpPathId,43,-1491.57,-2450.42,53.6645,0,0,0,0,100,0),
(@wpPathId,44,-1473.67,-2435,56.438,0,0,0,0,100,0),
(@wpPathId,45,-1458.8,-2420.24,58.7543,0,0,0,0,100,0),
(@wpPathId,46,-1451.61,-2406.21,60.4971,0,0,0,0,100,0),
(@wpPathId,47,-1450.24,-2390.07,61.2988,0,0,0,0,100,0),
(@wpPathId,48,-1449.98,-2369.08,61.4719,0,0,0,0,100,0),
(@wpPathId,49,-1450.01,-2348.08,61.5948,0,0,0,0,100,0),
(@wpPathId,50,-1450.28,-2327.09,61.7184,0,0,0,0,100,0),
(@wpPathId,51,-1449.22,-2302.62,61.8278,0,0,0,0,100,0),
(@wpPathId,52,-1445.48,-2285.57,62.1128,0,0,0,0,100,0),
(@wpPathId,53,-1438.68,-2273.44,63.0625,0,0,0,0,100,0),
(@wpPathId,54,-1427.75,-2259.8,63.363,0,0,0,0,100,0),
(@wpPathId,55,-1417.94,-2245.39,63.7294,0,0,0,0,100,0),
(@wpPathId,56,-1411.22,-2229.24,63.7811,0,0,0,0,100,0),
(@wpPathId,57,-1406.21,-2212.48,63.8356,0,0,0,0,100,0),
(@wpPathId,58,-1399.67,-2185.27,63.9419,0,0,0,0,100,0),
(@wpPathId,59,-1395.07,-2161.21,64.1007,0,0,0,0,100,0),
(@wpPathId,60,-1388.98,-2130.31,64.2996,0,0,0,0,100,0),
(@wpPathId,61,-1385.75,-2095.48,63.6847,0,0,0,0,100,0),
(@wpPathId,62,-1383.99,-2067.53,62.3167,0,0,0,0,100,0),
(@wpPathId,63,-1381.89,-2032.6,60.8356,0,0,0,0,100,0),
(@wpPathId,64,-1379.07,-2001.23,59.3176,0,0,0,0,100,0),
(@wpPathId,65,-1376.39,-1980.42,58.542,0,0,0,0,100,0),
(@wpPathId,66,-1373.23,-1970.43,58.49,0,0,0,0,100,0),
(@wpPathId,67,-1362.48,-1944.58,58.4418,0,0,0,0,100,0),
(@wpPathId,68,-1350.37,-1915.5,58.3884,0,0,0,0,100,0),
(@wpPathId,69,-1338.43,-1882.6,60.3171,0,0,0,0,100,0),
(@wpPathId,70,-1329.51,-1856.07,62.228,0,0,0,0,100,0),
(@wpPathId,71,-1325.86,-1842.56,63.0927,0,0,0,0,100,0),
(@wpPathId,72,-1316,-1825.72,64.4425,0,0,0,0,100,0),
(@wpPathId,73,-1306.65,-1815.35,65.3555,0,0,0,0,100,0),
(@wpPathId,74,-1289.41,-1803.39,66.6877,0,0,0,0,100,0),
(@wpPathId,75,-1261.87,-1788.14,66.6785,0,0,0,0,100,0),
(@wpPathId,76,-1249.45,-1783.07,65.8931,0,0,0,0,100,0),
(@wpPathId,77,-1238.49,-1775.65,64.62,0,0,0,0,100,0),
(@wpPathId,78,-1215.06,-1754.6,60.4524,0,0,0,0,100,0),
(@wpPathId,79,-1194.54,-1735.55,57.2315,0,0,0,0,100,0),
(@wpPathId,80,-1172.04,-1719.07,54.128,0,0,0,0,100,0),
(@wpPathId,81,-1159.31,-1713.28,52.2178,0,0,0,0,100,0),
(@wpPathId,82,-1139.35,-1706.8,48.8841,0,0,0,0,100,0),
(@wpPathId,83,-1115.81,-1700,45.5234,0,0,0,0,100,0),
(@wpPathId,84,-1092.46,-1692.58,41.2765,0,0,0,0,100,0),
(@wpPathId,85,-1068.59,-1687.37,38.652,0,0,0,0,100,0),
(@wpPathId,86,-1037.27,-1684.41,36.8739,0,0,0,0,100,0),
(@wpPathId,87,-1008.56,-1684.45,37.151,0,0,0,0,100,0),
(@wpPathId,88,-980.565,-1684.34,37.7322,0,0,0,0,100,0),
(@wpPathId,89,-953.792,-1680.65,39.8905,0,0,0,0,100,0),
(@wpPathId,90,-937.298,-1674.86,41.9623,0,0,0,0,100,0),
(@wpPathId,91,-922.117,-1666.19,44.3338,0,0,0,0,100,0),
(@wpPathId,92,-902.794,-1651.14,47.3033,0,0,0,0,100,0),
(@wpPathId,93,-886.795,-1637.57,49.7302,0,0,0,0,100,0),
(@wpPathId,94,-874.432,-1625.18,51.403,0,0,0,0,100,0),
(@wpPathId,95,-861.587,-1613.31,52.861,0,0,0,0,100,0),
(@wpPathId,96,-851.445,-1603.67,53.5052,0,0,0,0,100,0),
(@wpPathId,97,-835.526,-1585.06,54.2578,0,0,0,0,100,0),
(@wpPathId,98,-822.556,-1568.54,54.1683,0,0,0,0,100,0),
(@wpPathId,99,-804.126,-1543,54.2734,0,0,0,0,100,0),
(@wpPathId,100,-788.23,-1519.96,55.9301,0,0,0,0,100,0),
(@wpPathId,101,-779.762,-1504.66,57.6297,0,0,0,0,100,0),
(@wpPathId,102,-771.293,-1481.79,60.9203,0,0,0,0,100,0),
(@wpPathId,103,-764.958,-1461.8,63.8378,0,0,0,0,100,0),
(@wpPathId,104,-751.162,-1433.5,66.2523,0,0,0,0,100,0),
(@wpPathId,105,-737.279,-1406.06,68.0082,0,0,0,0,100,0),
(@wpPathId,106,-719.827,-1384.18,68.2183,0,0,0,0,100,0),
(@wpPathId,107,-701.185,-1358.79,67.6137,0,0,0,0,100,0),
(@wpPathId,108,-676.3,-1329.47,67.0156,0,0,0,0,100,0),
(@wpPathId,109,-661.743,-1308.94,66.136,0,0,0,0,100,0),
(@wpPathId,110,-646.567,-1281.38,66.0949,0,0,0,0,100,0),
(@wpPathId,111,-626.252,-1248.74,66.0719,0,0,0,0,100,0),
(@wpPathId,112,-612.39,-1216.62,65.9958,0,0,0,0,100,0),
(@wpPathId,113,-600.363,-1191.33,66.0663,0,0,0,0,100,0),
(@wpPathId,114,-587.601,-1158.84,66.2411,0,0,0,0,100,0),
(@wpPathId,115,-580.715,-1135.33,65.9731,0,0,0,0,100,0),
(@wpPathId,116,-569.879,-1109.61,63.4267,0,0,0,0,100,0),
(@wpPathId,117,-559.217,-1094.17,60.3274,0,0,0,0,100,0),
(@wpPathId,118,-539.599,-1074.21,56.4081,0,0,0,0,100,0),
(@wpPathId,119,-523.011,-1056.35,52.3473,0,0,0,0,100,0),
(@wpPathId,120,-510.191,-1040.07,48.6025,0,0,0,0,100,0),
(@wpPathId,121,-501.715,-1022.04,44.2363,0,0,0,0,100,0),
(@wpPathId,122,-496.385,-998.154,40.2856,0,0,0,0,100,0),
(@wpPathId,123,-494.702,-980.738,37.5433,0,0,0,0,100,0),
(@wpPathId,124,-491.376,-960.06,33.8258,0,0,0,0,100,0),
(@wpPathId,125,-487.913,-946.496,34.5604,0,0,0,0,100,0),
(@wpPathId,126,-483.667,-929.517,33.981,0,0,0,0,100,0),
(@wpPathId,127,-479.713,-916.112,34.5368,0,0,0,0,100,0),
(@wpPathId,128,-468.995,-898.097,37.5774,0,0,0,0,100,0),
(@wpPathId,129,-452.089,-875.795,43.0344,0,0,0,0,100,0),
(@wpPathId,130,-436.879,-856.588,47.9147,0,0,0,0,100,0),
(@wpPathId,131,-421.433,-837.576,52.3119,0,0,0,0,100,0),
(@wpPathId,132,-408.891,-820.775,53.7396,0,0,0,0,100,0),
(@wpPathId,133,-396.945,-802.835,54.5859,0,0,0,0,100,0),
(@wpPathId,134,-383.2,-783.4,54.5997,0,0,0,0,100,0),
(@wpPathId,135,-375.514,-777.305,54.4699,0,0,0,0,100,0),
(@wpPathId,136,-357.159,-768.896,54.1852,0,0,0,0,100,0),
(@wpPathId,137,-331.407,-757.909,53.8287,0,0,0,0,100,0),
(@wpPathId,138,-301.818,-747.177,55.4595,0,0,0,0,100,0),
(@wpPathId,139,-275.645,-739.098,56.6931,0,0,0,0,100,0),
(@wpPathId,140,-248.942,-730.377,58.3106,0,0,0,0,100,0),
(@wpPathId,141,-217.832,-723.297,61.0932,0,0,0,0,100,0),
(@wpPathId,142,-196.602,-718.715,62.8802,0,0,0,0,100,0),
(@wpPathId,143,-172.911,-715.793,63.6936,0,0,0,0,100,0),
(@wpPathId,144,-137.922,-715.202,64.7113,0,0,0,0,100,0),
(@wpPathId,145,-105.48,-713.642,65.4473,0,0,0,0,100,0),
(@wpPathId,146,-83.076,-715.929,67.8031,0,0,0,0,100,0),
(@wpPathId,147,-62.1287,-717.276,68.264,0,0,0,0,100,0),
(@wpPathId,148,-34.829,-715.287,69.076,0,0,0,0,100,0),
(@wpPathId,149,-20.5953,-718.476,69.3706,0,0,0,0,100,0),
(@wpPathId,150,-17.5473,-725.856,68.4891,0,0,0,0,100,0),
(@wpPathId,151,-18.4696,-738.195,66.1096,0,0,0,0,100,0),
(@wpPathId,152,-22.5461,-758.719,62.5768,0,0,0,0,100,0),
(@wpPathId,153,-16.6739,-786.375,59.6202,0,0,0,0,100,0),
(@wpPathId,154,-17.4867,-799.341,58.967,0,0,0,0,100,0),
(@wpPathId,155,-28.1695,-814.096,57.6642,0,0,0,0,100,0),
(@wpPathId,156,-44.6354,-826.165,57.0593,0,0,0,0,100,0),
(@wpPathId,157,-52.8964,-844.203,56.3922,0,0,0,0,100,0),
(@wpPathId,158,-50.6344,-863.401,55.686,0,0,0,0,100,0),
(@wpPathId,159,-41.1772,-876.152,55.8108,0,0,0,0,100,0),
(@wpPathId,160,-28.9852,-888.752,56.1343,0,0,0,0,100,0),
(@wpPathId,161,-26.666,-897.974,56.026,0,0,0,0,100,0),
(@wpPathId,162,-29.7521,-903.466,55.8896,0,0,0,0,100,0),
(@wpPathId,163,-33.5225,-909.703,55.5687,0,0,0,0,100,0),
(@wpPathId,164,-40.2458,-919.112,54.8082,0,0,0,0,100,0), -- destination reached, 1 minute delay before pathing back
(@wpPathId,165,-37.9023,-916.188,55.1725,0,0,0,0,100,0),
(@wpPathId,166,-30.3595,-904.044,55.8805,0,0,0,0,100,0),
(@wpPathId,167,-27.4496,-898.895,56.0401,0,0,0,0,100,0),
(@wpPathId,168,-28.8969,-890.134,56.0851,0,0,0,0,100,0),
(@wpPathId,169,-39.6026,-878.277,55.9009,0,0,0,0,100,0),
(@wpPathId,170,-49.9883,-864.653,55.6548,0,0,0,0,100,0),
(@wpPathId,171,-53.5191,-847.529,56.3265,0,0,0,0,100,0),
(@wpPathId,172,-45.6486,-827.843,57.0118,0,0,0,0,100,0),
(@wpPathId,173,-30.2519,-815.667,57.6413,0,0,0,0,100,0),
(@wpPathId,174,-18.748,-801.928,58.75,0,0,0,0,100,0),
(@wpPathId,175,-16.2843,-788.799,59.5083,0,0,0,0,100,0),
(@wpPathId,176,-22.8308,-761.081,62.172,0,0,0,0,100,0),
(@wpPathId,177,-18.7154,-740.543,65.6946,0,0,0,0,100,0),
(@wpPathId,178,-17.5494,-726.441,68.377,0,0,0,0,100,0),
(@wpPathId,179,-21.3868,-718.702,69.3223,0,0,0,0,100,0),
(@wpPathId,180,-34.1316,-715.917,69.0751,0,0,0,0,100,0),
(@wpPathId,181,-60.9359,-717.195,68.3029,0,0,0,0,100,0),
(@wpPathId,182,-80.9546,-715.894,67.9737,0,0,0,0,100,0),
(@wpPathId,183,-106.424,-713.731,65.3761,0,0,0,0,100,0),
(@wpPathId,184,-137.902,-714.769,64.7226,0,0,0,0,100,0),
(@wpPathId,185,-172.878,-715.923,63.6948,0,0,0,0,100,0),
(@wpPathId,186,-193.71,-718.413,63.0221,0,0,0,0,100,0),
(@wpPathId,187,-216.695,-722.939,61.1133,0,0,0,0,100,0),
(@wpPathId,188,-247.633,-730.432,58.4478,0,0,0,0,100,0),
(@wpPathId,189,-273.719,-738.461,56.7533,0,0,0,0,100,0),
(@wpPathId,190,-299.708,-746.781,55.6276,0,0,0,0,100,0),
(@wpPathId,191,-329.496,-757.193,53.9239,0,0,0,0,100,0),
(@wpPathId,192,-354.54,-768.04,54.2285,0,0,0,0,100,0),
(@wpPathId,193,-375.293,-777.319,54.4701,0,0,0,0,100,0),
(@wpPathId,194,-383.495,-784.036,54.6087,0,0,0,0,100,0),
(@wpPathId,195,-396.589,-802.639,54.5903,0,0,0,0,100,0),
(@wpPathId,196,-409.141,-821.598,53.6989,0,0,0,0,100,0),
(@wpPathId,197,-420.379,-835.882,52.5394,0,0,0,0,100,0),
(@wpPathId,198,-434.7,-853.557,48.7153,0,0,0,0,100,0),
(@wpPathId,199,-451.476,-875.092,43.2386,0,0,0,0,100,0),
(@wpPathId,200,-468.216,-896.654,37.8402,0,0,0,0,100,0),
(@wpPathId,201,-479.431,-916.404,34.5483,0,0,0,0,100,0),
(@wpPathId,202,-483.477,-929.436,34.0016,0,0,0,0,100,0),
(@wpPathId,203,-486.836,-942.665,34.6166,0,0,0,0,100,0),
(@wpPathId,204,-490.311,-955.864,33.8972,0,0,0,0,100,0),
(@wpPathId,205,-494.413,-978.225,37.0571,0,0,0,0,100,0),
(@wpPathId,206,-495.922,-996.353,39.9845,0,0,0,0,100,0),
(@wpPathId,207,-501.894,-1021.3,44.1397,0,0,0,0,100,0),
(@wpPathId,208,-508.274,-1036.2,47.7283,0,0,0,0,100,0),
(@wpPathId,209,-521.649,-1054.57,52.0204,0,0,0,0,100,0),
(@wpPathId,210,-536.872,-1071.44,56.0008,0,0,0,0,100,0),
(@wpPathId,211,-556.428,-1090.44,59.4959,0,0,0,0,100,0),
(@wpPathId,212,-569.52,-1109.03,63.3078,0,0,0,0,100,0),
(@wpPathId,213,-580.307,-1134,65.9386,0,0,0,0,100,0),
(@wpPathId,214,-587.943,-1159.8,66.2384,0,0,0,0,100,0),
(@wpPathId,215,-599.68,-1189.38,66.071,0,0,0,0,100,0),
(@wpPathId,216,-611.207,-1214.11,66.0131,0,0,0,0,100,0),
(@wpPathId,217,-625.737,-1247.48,66.0764,0,0,0,0,100,0),
(@wpPathId,218,-644.589,-1278.51,66.0956,0,0,0,0,100,0),
(@wpPathId,219,-660.145,-1306.27,66.1604,0,0,0,0,100,0),
(@wpPathId,220,-675.672,-1328.66,66.9782,0,0,0,0,100,0),
(@wpPathId,221,-699.123,-1356.5,67.6145,0,0,0,0,100,0),
(@wpPathId,222,-718.071,-1382.09,68.1733,0,0,0,0,100,0),
(@wpPathId,223,-737.753,-1407.11,67.967,0,0,0,0,100,0),
(@wpPathId,224,-750.189,-1431.38,66.3932,0,0,0,0,100,0),
(@wpPathId,225,-764.174,-1459.99,64.0482,0,0,0,0,100,0),
(@wpPathId,226,-771.283,-1481.56,60.9477,0,0,0,0,100,0),
(@wpPathId,227,-778.669,-1502.06,58.0151,0,0,0,0,100,0),
(@wpPathId,228,-786.991,-1518,56.1296,0,0,0,0,100,0),
(@wpPathId,229,-802.538,-1540.44,54.3658,0,0,0,0,100,0),
(@wpPathId,230,-821.021,-1566.36,54.1669,0,0,0,0,100,0),
(@wpPathId,231,-834.994,-1584.29,54.2546,0,0,0,0,100,0),
(@wpPathId,232,-851.463,-1603.03,53.5575,0,0,0,0,100,0),
(@wpPathId,233,-861.063,-1612.73,52.8747,0,0,0,0,100,0),
(@wpPathId,234,-871.01,-1622.07,52.0091,0,0,0,0,100,0),
(@wpPathId,235,-884.087,-1634.72,49.907,0,0,0,0,100,0),
(@wpPathId,236,-901.22,-1649.65,47.6492,0,0,0,0,100,0),
(@wpPathId,237,-919.01,-1663.83,44.7569,0,0,0,0,100,0),
(@wpPathId,238,-936.362,-1674.18,42.1281,0,0,0,0,100,0),
(@wpPathId,239,-951.672,-1680.22,40.0861,0,0,0,0,100,0),
(@wpPathId,240,-978.641,-1684.29,37.8759,0,0,0,0,100,0),
(@wpPathId,241,-1004.88,-1684.59,37.0962,0,0,0,0,100,0),
(@wpPathId,242,-1036.73,-1684.53,36.8569,0,0,0,0,100,0),
(@wpPathId,243,-1063.92,-1686.7,38.2989,0,0,0,0,100,0),
(@wpPathId,244,-1090.65,-1692.18,40.9512,0,0,0,0,100,0),
(@wpPathId,245,-1112.34,-1698.96,45.1239,0,0,0,0,100,0),
(@wpPathId,246,-1138.5,-1706.74,48.6789,0,0,0,0,100,0),
(@wpPathId,247,-1155.85,-1712.21,51.8194,0,0,0,0,100,0),
(@wpPathId,248,-1171.71,-1719.06,54.0856,0,0,0,0,100,0),
(@wpPathId,249,-1193.8,-1735,57.0706,0,0,0,0,100,0),
(@wpPathId,250,-1213.93,-1753.41,60.187,0,0,0,0,100,0),
(@wpPathId,251,-1237.63,-1774.69,64.4626,0,0,0,0,100,0),
(@wpPathId,252,-1248.78,-1782.43,65.819,0,0,0,0,100,0),
(@wpPathId,253,-1258.66,-1786.65,66.482,0,0,0,0,100,0),
(@wpPathId,254,-1286.65,-1801.77,66.8709,0,0,0,0,100,0),
(@wpPathId,255,-1305.07,-1814.08,65.4825,0,0,0,0,100,0),
(@wpPathId,256,-1315.82,-1825.3,64.4726,0,0,0,0,100,0),
(@wpPathId,257,-1325.03,-1840.48,63.2467,0,0,0,0,100,0),
(@wpPathId,258,-1328.78,-1852.54,62.4244,0,0,0,0,100,0),
(@wpPathId,259,-1338.59,-1882.78,60.2984,0,0,0,0,100,0),
(@wpPathId,260,-1349.41,-1912.74,58.414,0,0,0,0,100,0),
(@wpPathId,261,-1361.58,-1942.17,58.429,0,0,0,0,100,0),
(@wpPathId,262,-1372.17,-1967.33,58.476,0,0,0,0,100,0),
(@wpPathId,263,-1376.43,-1980.29,58.5393,0,0,0,0,100,0),
(@wpPathId,264,-1378.99,-1998.28,59.1872,0,0,0,0,100,0),
(@wpPathId,265,-1381.75,-2030.01,60.7155,0,0,0,0,100,0),
(@wpPathId,266,-1383.96,-2066.34,62.2794,0,0,0,0,100,0),
(@wpPathId,267,-1385.64,-2093.57,63.5406,0,0,0,0,100,0),
(@wpPathId,268,-1389.12,-2129.8,64.3013,0,0,0,0,100,0),
(@wpPathId,269,-1395.05,-2161.59,64.1007,0,0,0,0,100,0),
(@wpPathId,270,-1399.36,-2183.92,63.9526,0,0,0,0,100,0),
(@wpPathId,271,-1405.66,-2210.48,63.8462,0,0,0,0,100,0),
(@wpPathId,272,-1410.75,-2227.95,63.7957,0,0,0,0,100,0),
(@wpPathId,273,-1417.65,-2244.76,63.7518,0,0,0,0,100,0),
(@wpPathId,274,-1427.39,-2259.19,63.3891,0,0,0,0,100,0),
(@wpPathId,275,-1438.66,-2273.46,63.0608,0,0,0,0,100,0),
(@wpPathId,276,-1444.57,-2283.39,62.2072,0,0,0,0,100,0),
(@wpPathId,277,-1449.32,-2302.21,61.8304,0,0,0,0,100,0),
(@wpPathId,278,-1450.22,-2324.94,61.7334,0,0,0,0,100,0),
(@wpPathId,279,-1450.08,-2347.68,61.5991,0,0,0,0,100,0),
(@wpPathId,280,-1449.96,-2365.88,61.4915,0,0,0,0,100,0),
(@wpPathId,281,-1450.25,-2388.62,61.3176,0,0,0,0,100,0),
(@wpPathId,282,-1450.85,-2402.24,60.7831,0,0,0,0,100,0),
(@wpPathId,283,-1457.88,-2418.95,58.9511,0,0,0,0,100,0),
(@wpPathId,284,-1473.83,-2435.1,56.4162,0,0,0,0,100,0),
(@wpPathId,285,-1491.02,-2449.99,53.7364,0,0,0,0,100,0),
(@wpPathId,286,-1505.01,-2461.63,52.9892,0,0,0,0,100,0),
(@wpPathId,287,-1519.04,-2473.22,53.2747,0,0,0,0,100,0),
(@wpPathId,288,-1536.46,-2487.83,53.5903,0,0,0,0,100,0),
(@wpPathId,289,-1545.19,-2496.39,54.1743,0,0,0,0,100,0),
(@wpPathId,290,-1550.28,-2506.8,53.9085,0,0,0,0,100,0),
(@wpPathId,291,-1554.16,-2515.03,53.3954,0,0,0,0,100,0),
(@wpPathId,292,-1560.37,-2527.18,52.3382,0,0,0,0,100,0),
(@wpPathId,293,-1567.58,-2549.4,50.7789,0,0,0,0,100,0),
(@wpPathId,294,-1567.29,-2570.35,49.3619,0,0,0,0,100,0),
(@wpPathId,295,-1562.37,-2591.16,48.5309,0,0,0,0,100,0),
(@wpPathId,296,-1557.69,-2603.97,47.8564,0,0,0,0,100,0),
(@wpPathId,297,-1555.93,-2619.39,45.2651,0,0,0,0,100,0),
(@wpPathId,298,-1555.27,-2642.13,41.156,0,0,0,0,100,0),
(@wpPathId,299,-1557.99,-2669.24,36.7433,0,0,0,0,100,0),
(@wpPathId,300,-1564.24,-2685.93,35.3788,0,0,0,0,100,0),
(@wpPathId,301,-1570.22,-2693.06,34.7322,0,0,0,0,100,0),
(@wpPathId,302,-1585.9,-2709.52,35.9957,0,0,0,0,100,0),
(@wpPathId,303,-1597.38,-2723.02,37.1346,0,0,0,0,100,0),
(@wpPathId,304,-1606.15,-2738.58,37.5876,0,0,0,0,100,0),
(@wpPathId,305,-1610.13,-2756.28,37.2692,0,0,0,0,100,0),
(@wpPathId,306,-1612.47,-2774.32,36.3171,0,0,0,0,100,0),
(@wpPathId,307,-1617.11,-2801.22,34.8703,0,0,0,0,100,0),
(@wpPathId,308,-1620.57,-2819.08,33.8744,0,0,0,0,100,0),
(@wpPathId,309,-1625.11,-2841.37,32.3162,0,0,0,0,100,0),
(@wpPathId,310,-1632.1,-2867.75,30.009,0,0,0,0,100,0),
(@wpPathId,311,-1638.53,-2884.76,27.7024,0,0,0,0,100,0),
(@wpPathId,312,-1644,-2901.43,26.3001,0,0,0,0,100,0),
(@wpPathId,313,-1641.88,-2915.77,26.7946,0,0,0,0,100,0),
(@wpPathId,314,-1637.81,-2933.46,28.1655,0,0,0,0,100,0),
(@wpPathId,315,-1638.86,-2944.22,28.5141,0,0,0,0,100,0),
(@wpPathId,316,-1643.12,-2959.2,27.6933,0,0,0,0,100,0),
(@wpPathId,317,-1645.97,-2971.19,26.3978,0,0,0,0,100,0),
(@wpPathId,318,-1645.94,-2988.94,23.7789,0,0,0,0,100,0),
(@wpPathId,319,-1640.24,-3005.1,20.8809,0,0,0,0,100,0),
(@wpPathId,320,-1634.33,-3012.31,19.4302,0,0,0,0,100,0),
(@wpPathId,321,-1604.85,-3044.09,14.9102,0,0,0,0,100,0),
(@wpPathId,322,-1584.7,-3059.58,12.848,0,0,0,0,100,0),
(@wpPathId,323,-1569.47,-3066.36,13.156,0,0,0,0,100,0),
(@wpPathId,324,-1559.23,-3066.54,13.1732,0,0,0,0,100,0),
(@wpPathId,325,-1551.49,-3056.18,13.011,0,0,0,0,100,0),
(@wpPathId,326,-1551.22,-3043.06,13.002,0,0,0,0,100,0),
(@wpPathId,327,-1552.52,-3029.32,13.6458,5.009628,0,0,0,100,0); -- ends at same coords as WP 1

-- Assign path and formation info
DELETE FROM `creature_formations` WHERE `leaderGUID`=@npcLeaderId;
INSERT INTO creature_formations (`leaderGUID`,`memberGUID`,`dist`,`angle`,`groupAI`,`point_1`,`point_2`) VALUES 
(@npcLeaderId,@npcLeaderId,0,0,2,0,0),
(@npcLeaderId,@npcFollowerId_1,3,180,2,0,0),
(@npcLeaderId,@npcFollowerId_2,3,90,2,0,0),
(@npcLeaderId,@npcFollowerId_3,3,360,2,0,0),
(@npcLeaderId,@npcFollowerId_4,3,270,2,0,0);

DELETE FROM `creature_addon` WHERE `guid`=@npcLeaderId;
INSERT INTO `creature_addon` (`guid`, `path_id`, `mount`, `bytes1`, `bytes2`, `emote`, `auras`) VALUES
(@npcLeaderId,@wpPathId,0,0,0,0,NULL);

-- Fix blood elf paladin visual
DELETE FROM `spell_linked_spell` WHERE `spell_trigger` IN (34767, 34769);
INSERT INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `comment`) VALUES
(34767, 31726, 'Summon Charger - Blood Elf'),
(34769, 31726, 'Summon Warhorse - Blood Elf');

-- Correct inhabittype for creatures that should chase into water
UPDATE `creature_template` SET `InhabitType`=3 WHERE `entry` IN (5931, 3989, 3988, 3991, 3999, 4004, 4003);

-- fix startup errors
UPDATE creature SET curhealth = 1950 WHERE guid=14575;
UPDATE creature SET curhealth = 1950 WHERE guid=14574;
UPDATE creature SET curmana = 1783 WHERE guid=14572;
