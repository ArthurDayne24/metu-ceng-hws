module Hw1 (next) where -- DO NOT CHANGE THIS

next :: [[String]] -> [[String]]
next table = doAlives (doKills table (whomToKill table)) (whomToAlive table)

isValid :: Int -> Int -> Int -> Int -> Bool
isValid x y rows columns = if rows>x && x>=0 && columns>y && y>=0 then True
				 else False

adjoints :: [[String]] -> Int -> Int -> [(Int, Int)]
adjoints table x y = [(i, j) | i<-[x+r | r<-rep], j<-[y+r | r<-rep],
					 	isValid i j (length table) (length (table!!0)),
						(i/=x || j/=y), table !! i !! j == "B"]
  					 where rep=[-1, 0, 1]

--Return  -1 if it should be killed,
--		   0 if no change should be done,
--		  +1 if you make it alive.

soWhat :: [[String]] -> Int -> Int -> Int
soWhat table x y =	case table !! x !! y of 
				"B" ->
					if val>3 || val<2 then -1 
					else 0                    
				"W" ->
					if val==3 then 1
					else 0

				where val=length(adjoints table x y)

doAlives :: [[String]] -> [(Int, Int)] -> [[String]]
doAlives table [] = table
doAlives table (x:xs) = doAlives 
						(
							( take (fst x) table ) ++ 
							[ ( take (snd x) row ) ++ ["B"] ++ ( after (snd x + 1 ) row ) ] ++ 
							(after (fst x + 1) table) 
						) xs
		where row=table !! (fst x)

doKills :: [[String]] -> [(Int, Int)] -> [[String]]
doKills table [] = table
doKills table (x:xs) = doKills 
					   ( 
					   		( take (fst x) table ) ++ 
							[( take (snd x) row ) ++ ["W"] ++ ( after (snd x + 1) row ) ] ++
							(after (fst x + 1) table) 
					   ) xs
		where row=table !! (fst x)

after :: Int -> [a] ->  [a]
after 0 xs = xs
after n xs = after (n-1) (tail xs) 

whomToKill :: [[String]] -> [(Int, Int)]
whomToKill table = [(x, y) | (x, y)<-coordinates, (soWhat table x y)== -1]
				where coordinates=[(i, j) | i<-[0..(length table)-1], j<-[0..(length (table!!0))-1]]

whomToAlive :: [[String]] -> [(Int, Int)]
whomToAlive table = [(x, y) | (x, y)<-coordinates, (soWhat table x y)==1]
				where coordinates=[(i, j) | i<-[0..(length table)-1], j<-[0..(length (table!!0))-1]]


-- DO NOT CHANGE CODE BELOW THIS LINE! --
createFile fileName = writeFile fileName ""

writeSteps filename grid n = do
                    createFile filename
                    writeStepsHelper filename grid n n
    where writeStepsHelper filename grid n all = if n == 0 then putStrLn ((show all) ++ " steps are succesfully written to \'" ++ filename ++ "\'") else do
                                    appendFile filename ((show (next grid)) ++ "\n")
                                    writeStepsHelper filename (next grid) (n-1) all
