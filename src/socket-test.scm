
(define server (make-socket))
(define clients '())

(define (start-server port backlog)
  (socket-bind server port)
  (socket-listen server backlog))

(define (com-loop)
  ;; Accept a new client if one is waiting.
  (if (not (null? (car (select '(server) '() '() 100))))
      (set! clients (cons (socket-accept server) clients)))
  ;; read all ready clients data.
  (let ((ready-clients (select clients '() '() 100)))
    (for-each (lambda (x) (read-client x)) (car ready-clients)))
  (com-loop))

(define (read-client c)
  (let ((input '()))
    (do () ((null? (car (select '(c) '() '() 1))))
      (set! input (cons (read-char c) input)))
    (display (list->string (reverse input)))))


