%:- module(hw5, [filled_area/2, filling_list/4]).
%:- [tiles].

% DO NOT MODIFY THE UPPER CONTENT, WRITE YOUR CODE AFTER THIS LINE
tile(t1, 3, 5, blue).
tile(t2, 1, 2, blue).
tile(t3, 4, 3, blue).
tile(t4, 2, 4, red).
tile(t5, 4, 1, red).
tile(t6, 2, 3, green).
tile(t7, 1, 5, green).
tile(t8, 3, 3, green).
tile(t9, 1, 1, purple).
tile(t10, 2 , 5, purple).

filled_area([], 0).
filled_area([X | XS], A) :-
    filled_area(XS, A1),
    tile(X, W, H, _),
    A is A1 + W * H.

filling_list(WA, HA, P, [X | XS]) :-
    tile(X, W, H, _),
    H =< HA, W =< WA,
    RW is WA - W,
    (
      (
        RW == 0, XS = [], WA * HA * P =< W * H
      );
      (
        RW > 0, RP is (WA * HA * P - W * H) / (RW * HA),
        filling_list(RW, HA, RP, XS),
        not(member(X, XS))
      )
    ).


















