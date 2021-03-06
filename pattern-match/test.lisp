(defun assert-equal (exp1 exp2)
  (if (equal exp1 exp2)
	  (print "Test passed.")
	  (print (list "Test failed." exp1))))

(defun runtest ()
  (assert-equal (pattern-match '(a * ?x) '(a b)) '((?x b)))
  (assert-equal (pattern-match '(a * ?x *) '(a b c d)) '(((?x  b)) ((?x  c)) ((?x  d))))
  (assert-equal (pattern-match '() '()) T)  
  (assert-equal (pattern-match '(ai) '(ai)) T)
  (assert-equal (pattern-match  '(ai cs) '(ai cs)) T)
  (assert-equal (pattern-match '(cs ai) '(ai cs)) NIL)
  (assert-equal (pattern-match '(1 2 3 0) '(1 2 3 4 0)) NIL)
  (assert-equal (pattern-match '(? mudd) '(seely mudd)) T)
  (assert-equal (pattern-match '(?first ?middle mudd) '(seely w mudd)) '((middle  w) (?first seely)))
  (assert-equal (pattern-match '(? ?x ? ?y ?) '(Warren Buffet Is A Good Man)) NIL)
  (assert-equal (pattern-match '(School Of Engineering and Applied Science) '(School Of Engineering)) NIL)
  (assert-equal (pattern-match '(* School Of Engineering and Applied Science) '(The Fu Foundation School Of Engineering and Applied Science)) T)
  (assert-equal (pattern-match '(The * School Of Engineering and Applied Science) '(The Fu Foundation School Of Engineering and Applied Science)) T)
  (assert-equal (pattern-match '(The * School Of Engineering and Applied Science) '(The School Of Engineering and Applied Science)) T)
  (assert-equal (pattern-match '(* 3 ?x 4 *) '(3 5 4)) '((?x  5)))
  (assert-equal (pattern-match '( ?x (1 2) ?y (4 5)) '(c (1 2) d (4 5))) '((?y  d) (?x  c)))
  (assert-equal (pattern-match '(?y ?z (c v)) '(8 gh (c v) )) '((?z  gh) (?y  8)))
  (assert-equal (pattern-match '(((get) me) out) '(get (me (out)))) NIL)
  (assert-equal (pattern-match '(A * B) '(A A A A A B)) T)
  (assert-equal (pattern-match '(?x * ?y) '(A A A A A B)) '((?x  a) (?y  b)))
  (assert-equal (pattern-match '(?x (?x)) '(1 (1))) '((?x  1)))
  (assert-equal (pattern-match '(?x 2 (?x)) '(3 2 (1))) NIL)
  (assert-equal (pattern-match '(1 (* ?x *)) '(1 (a (b c) d))) '(((?x  a)) ((?x  (b c))) ((?x  d)))))

(defun test-time ()
  (pattern-match '(a * ?x) '(a b))
  (pattern-match '(a * ?x *) '(a b c d))
  (pattern-match '() '()) 
  (pattern-match '(ai) '(ai))
  (pattern-match  '(ai cs) '(ai cs))
  (pattern-match '(cs ai) '(ai cs))
  (pattern-match '(1 2 3 0) '(1 2 3 4 0))
  (pattern-match '(? mudd) '(seely mudd))
  (pattern-match '(?first ?middle mudd) '(seely w mudd))
  (pattern-match '(? ?x ? ?y ?) '(Warren Buffet Is A Good Man))
  (pattern-match '(School Of Engineering and Applied Science) '(School Of Engineering))
  (pattern-match '(* School Of Engineering and Applied Science) '(The Fu Foundation School Of Engineering and Applied Science))
  (pattern-match '(The * School Of Engineering and Applied Science) '(The Fu Foundation School Of Engineering and Applied Science))
  (pattern-match '(The * School Of Engineering and Applied Science) '(The School Of Engineering and Applied Science))
  (pattern-match '(* 3 ?x 4 *) '(3 5 4))
  (pattern-match '( ?x (1 2) ?y (4 5)) '(c (1 2) d (4 5)))
  (pattern-match '(?y ?z (c v)) '(8 gh (c v) ))
  (pattern-match '(((get) me) out) '(get (me (out))))
  (pattern-match '(A * B) '(A A A A A B))
  (pattern-match '(?x * ?y) '(A A A A A B))
  (pattern-match '(?x (?x)) '(1 (1)))
  (pattern-match '(?x 2 (?x)) '(3 2 (1)))
  (pattern-match '(1 (* ?x *)) '(1 (a (b c) d))))


(defun runtesttime()
  (loop for i from 1 to 10000 do
	   (test-time)))
