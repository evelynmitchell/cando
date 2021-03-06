(in-package :sdf)

(defun write-sdf-to-file (aggregate filename)
  (with-open-file (fout filename :direction :output :if-exists :supersede)
    (write-sdf-stream aggregate fout)))

(defun write-sdf-stream (aggregate stream)
  (warn "write-sdf-stream doesn't handle charges properly yet")
  (format stream "CHEMBL1093087~%
                    3D~%
Structure written by MMmdl.~%")
  (let ((atom-to-index (make-hash-table)))
    (let* ((atoms (chem:map-atoms 'vector
                                  (lambda (atm) atm)
                                  aggregate))
           (bonds (chem:map-bonds 'vector
                                  (lambda (atm1 atm2 bond-order)
                                    (list atm1 atm2 bond-order))
                                  aggregate))
           (coordinates (make-array (* 3 (length atoms)) :element-type 'double-float)))
  (format stream "~3d~3d~3d~3d~3d~3d            999 V2000~%"
          (length atoms)
          (length bonds)
          0
          0
          0
          0)
      (loop for index below (length atoms)
            for atm = (elt atoms index) ;;get element from vector at position
            for pos = (chem:get-position atm)
            for coord-index = (* 3 index)
            do (setf (gethash atm atom-to-index) index) ;;key=atm value=index
            do (setf (elt coordinates coord-index) (geom:vx pos)
                     (elt coordinates (+ 1 coord-index)) (geom:vy pos)
                     (elt coordinates (+ 2 coord-index)) (geom:vz pos))
               (format stream "~10,4f~10,4f~10,4f ~2a  0  0  0  0  0  0~%"
                       (elt coordinates coord-index)
                       (elt coordinates (+ 1 coord-index)) 
                       (elt coordinates (+ 2 coord-index))
                       (string (chem:get-element atm))))
      (chem:map-bonds nil
                      (lambda (a1 a2 order)
                        (let ((order-num (case order
                                           (:single-bond 1)
                                           (:double-bond 2)
                                           (:triple-bond 3)
                                           (otherwise 1))))
                          (format stream "~3d~3d~3d~3d~3d~3d~%"
                                  (1+ (gethash a1 atom-to-index))
                                  (1+ (gethash a2 atom-to-index))
                                  order-num
                                  0
                                  0
                                  0)))
                      aggregate)
      (format stream "M  END
> <ID>
CHEMBL1078774

> <IC50[nM]>
17

$$$$~%"))))
                
                        
