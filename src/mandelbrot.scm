
(do ((y -1.0 (+ y 0.1))) ((> y 1.1))
  (do ((x -2.0 (+ x 0.04))) ((> x 1.0))
    (let ((c 126)
          (z (make-rectangular x y)))
      (let ((a z))
       

    
    
    
(loop for y from -1 to 1.1 by 0.1 do
      (loop for x from -2 to 1 by 0.04 do
            (let* (
                   (c 126)
                   (z (complex x y))
                   (a z))
              (loop while (< (abs(setq z (+ (* z z) a))) 2)
                    while (> (decf c) 32)) 
              (princ (code-char c))))
      (format t "~%")) ; stream t is a standard output 

