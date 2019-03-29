module WordTree(WordTree(Word, Subword, Root),
    emptyTree, getAllPaths, addWords, deleteWords, getWordsBeginWith) where

data WordTree = Word String | Subword String [WordTree] | Root [WordTree]

emptyTree :: WordTree
emptyTree = Root []
-- DO NOT MODIFY ABOVE

getAllPaths :: WordTree -> [[String]]
addWords :: WordTree -> [String] -> WordTree
deleteWords :: WordTree -> [String] -> WordTree
getWordsBeginWith :: WordTree -> String -> [String]

--------------------------------------------------------------------------------------------
instance Show WordTree where
    show (Root branches) = concatMap ( \x -> showWordTree 1 "" x ) branches
        where showWordTree :: Int -> String -> WordTree -> String
              showWordTree n carry (Word s) = indent (2*n-2) ++ carry ++ s ++ "\n"
              showWordTree n carry (Subword s tree) = indent (2*n-2) ++ carry ++ s ++ ":\n" ++
                  concatMap ( \x -> showWordTree (n+1) (carry ++ s) x ) tree
              indent :: Int -> String
              indent n | n == 0 = ""
                       | otherwise = ' ':(indent (n-1))

--------------------------------------------------------------------------------------------

getAllPaths (Root branches) = concatMap ( \tree -> getAllPathsHelper [] tree ) branches

getAllPathsHelper :: [String] -> WordTree -> [[String]]
getAllPathsHelper carry (Word s) = [carry ++ [s]]
getAllPathsHelper carry (Subword s tree) =
    concatMap ( \x -> getAllPathsHelper ( carry ++ [s] ) x ) tree

--------------------------------------------------------------------------------------------

addWords tree@(Root _) strs = foldl addWord tree strs

addWord :: WordTree -> String -> WordTree
addWord tree@(Word s) str = if s == str then tree
                            else if suf1 < suf2 then Subword pre [Word suf1, Word suf2]
                            else Subword pre [Word suf2, Word suf1]
                                where (pre, suf1, suf2) = findPreAndSuffixs s str
addWord tree@(Subword s branches) str =
    let ls = length s
        lstr = length str
     in case  ls `compare` lstr of
          EQ ->
              if s == str then
                  if strOf (head branches) == "" then  tree
                  else Subword s ( ( Word "" ) : branches )
              else newTree
          GT ->
              newTree
          LT ->
              if take ls str == s then
                  let posTrees  = appropriate suf2 branches
                      lposTrees = length posTrees
                   in if lposTrees /= 0 then
                          let  subTree   = head posTrees
                               lbranches = length branches
                           in Subword s ( take ( lbranches-lposTrees ) branches ++
                            addWord subTree suf2 : tail posTrees )
                      else Subword s ( insert' (Word suf2) branches )
              else newTree
    where (pre, suf1, suf2) = findPreAndSuffixs s str
          newTree           = if suf1 < suf2
                                  then Subword pre [Subword suf1 branches, Word suf2]
                              else Subword pre [Word suf2, Subword suf1 branches]
addWord tree@(Root branches) str =
    if str == "" && length branches /= 0 && strOf ( head branches ) == "" then tree
    else
        let posTrees  = appropriate str branches
            lposTrees = length posTrees
         in if length posTrees /= 0 then
                let subTree   = head posTrees
                    lbranches = length branches
                 in Root ( take ( lbranches - lposTrees ) branches
                             ++ addWord subTree str : tail posTrees )
            else Root (insert' (Word str) branches)

--Check if any appropriate branch that may accept str exists.
--If not, return [], else a list keeping the branch at zeroth position
appropriate :: String -> [WordTree] -> [WordTree]
appropriate str branches =
    dropWhile ( \ tree -> [] == findPrefix str (strOf tree) ) branches

--Insert a tree into a list of trees, named branches
insert' :: WordTree -> [WordTree] -> [WordTree]
insert' tree branches = if length branches == 0 then [tree]
                        else
                            if strOf tree > strOf (head branches)
                                then head branches : insert' tree (tail branches)
                            else tree : branches

--------------------------------------------------------------------------------------------

deleteWords tree@(Root _) strs = foldl deleteWord tree strs

--Deletes str from the appropriate branch recursively
deleteWord :: WordTree -> String -> WordTree
deleteWord tree@(Root branches) str =
    if str == "" then
       if length branches /= 0 && strOf ( head branches ) == ""
           then Root $ tail branches
       else tree
    else
        let posTrees  = toWhichBranch str branches
            lbranches = length branches
            lposTrees = length posTrees
         in if lposTrees == 0 then tree
            else let subTree     = head posTrees
                     (_, suf, _) = findPreAndSuffixs str (strOf subTree)
                  in case subTree of
                       (Word s1)     ->
                              if str == s1
                                 then Root (take (lbranches-lposTrees) branches ++ tail posTrees)
                              else tree
                       (Subword _ _) ->
                              Root (take (lbranches-lposTrees) branches
                             ++ (deleteWord subTree suf) : tail posTrees)
deleteWord tree@(Subword s branches) str =
    let posTrees  = toWhichBranch str branches
        lbranches = length branches
        lposTrees = length posTrees
     in if lposTrees == 0 then tree
        else let subTree     = head posTrees
                 (_, suf, _) = findPreAndSuffixs str (strOf subTree)
              in case lbranches of
                    2 ->
                        case (head branches, branches !! 1) of
                          (Word _, Word _)           ->
                              if strOf subTree == str
                                 then Word ( s ++ strOf ( branches !! (lposTrees-1) ) )
                              else tree
                          (Subword _ _, Subword _ _) ->
                              Subword s (take (lbranches-lposTrees) branches ++
                                         (deleteWord subTree suf) : tail posTrees)
                          (   _   ,   _   )          ->
                              case subTree of
                                (Word _)  -> if strOf subTree == str
                                                then let rem = branches !! (lposTrees-1)
                                                      in Subword (s ++ strOf rem)
                                                                (branchesOf rem)
                                            else tree
                                (   _   ) ->
                                    Subword s (take (lbranches-lposTrees) branches ++
                                             (deleteWord subTree suf) : tail posTrees)
                    _ ->
                        case subTree of
                            (Word s1)     ->
                                   if str == s1
                                      then Subword s (take (lbranches-lposTrees) branches
                                                                          ++ tail posTrees)
                                   else tree
                            (Subword _ _) ->
                                   Subword s (take (lbranches-lposTrees) branches
                                       ++ (deleteWord subTree suf) : tail posTrees)

--------------------------------------------------------------------------------------------

getWordsBeginWith tree@(Root _) [] = map concat $ getAllPaths tree
getWordsBeginWith (Root branches) prefix = goOnWithBranches prefix branches

getWordsBeginWithHelper :: String -> WordTree -> [String]
getWordsBeginWithHelper prefix (Word s) = if s `beginsWith` prefix then [s] else []
getWordsBeginWithHelper prefix tree@(Subword s branches) =
    let pair = partition (length s) prefix in
        if s `beginsWith` (fst pair) then
            if (snd pair) ==  "" then map concat $ getAllPathsHelper [] tree
            else let wordsFromBranches = goOnWithBranches (snd pair) branches
                  in if length wordsFromBranches == 0 then []
                     else map ( \ x -> fst pair ++ x ) wordsFromBranches
        else []

--To which branch should we orient to find the words(if any) startting with prefix
goOnWithBranches :: String -> [WordTree] -> [String]
goOnWithBranches prefix branches =
    let posTrees = toWhichBranch prefix branches
     in if length posTrees == 0 then []
        else let found = getWordsBeginWithHelper prefix (head posTrees)
              in if length found == 0 then []
                 else found

-----------------------------------common helper functions----------------------------------

strOf :: WordTree -> String
strOf (Word s) = s
strOf (Subword s _) = s

--(Prefix, suffix of first, suffix of second)
findPreAndSuffixs :: String -> String -> (String, String, String)
findPreAndSuffixs [] y = ([], [], y)
findPreAndSuffixs x [] = ([], x, [])
findPreAndSuffixs s1@(x:xs) s2@(y:ys) = if x == y then (x:pre, suf1, suf2) else ([], s1, s2)
    where (pre, suf1, suf2) = findPreAndSuffixs xs ys

findPrefix :: String -> String -> String
findPrefix [] y = []
findPrefix x [] = []
findPrefix (x:xs) (y:ys) = if x == y then x:pre else []
    where pre = findPrefix xs ys

branchesOf :: WordTree -> [WordTree]
branchesOf (Subword _ b) = b
branchesOf (Root b) = b

--s `beginsWith` sub
beginsWith :: String -> String -> Bool
beginsWith _  [] = True
beginsWith [] _  = False
beginsWith s@(y:ys) sub@(x:xs) = x == y && beginsWith ys xs

--------------------sub procedures for getWordsBeginWith and deleteWords--------------------

--Generate partition of s into a tuple starting with length (l, len(s)-l)
partition :: Int -> String -> (String, String)
partition l s = splitAt m s where m = min l $ length s

--To which branch should we orient to find the words(if any) startting with prefix
toWhichBranch :: String -> [WordTree] -> [WordTree]
toWhichBranch prefix branches =
    dropWhile ( \ tree -> fails ( (strOf tree) `divide` prefix) ) branches
    where
         --Divide prefix into a pre-suff pair where pre = prefixOf(s, prefix)
         divide :: String -> String -> Maybe (String, String)
         divide s prefix =
             let pair = partition (length s) prefix
              in if s `beginsWith` (fst pair) && (fst pair /= "" || snd pair == "")
                    then Just pair
                 else Nothing
         fails :: Maybe (String, String) -> Bool
         fails Nothing = True
         fails _       = False
