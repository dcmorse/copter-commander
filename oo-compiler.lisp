;;;;; Copter Commander - A 2d network helicopter game
;;;;; Copyright (C) 20001  David Morse
;;;;; 
;;;;; This program is free software; you can redistribute it and/or modify
;;;;; it under the terms of the GNU General Public License as published by
;;;;; the Free Software Foundation; either version 2 of the License, or
;;;;; (at your option) any later version.
;;;;; 
;;;;; This program is distributed in the hope that it will be useful,
;;;;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;;;;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;;;;; GNU General Public License for more details.
;;;;; 
;;;;; You should have received a copy of the GNU General Public License
;;;;; along with this program; if not, write to the Free Software
;;;;; Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

;;;;; oo-compiler.lisp is a tool for reading a lisplike description of
;;;;; objects, and spitting out c parseable headers.  It manages single
;;;;; inheritance, generic functions, methods, per-instance and per-class
;;;;; variables.  It is only really used for sobjects, server-objects that
;;;;; represent copter-commander units.  There are a few very specific
;;;;; features that you don't find in other object systems, like (I think)
;;;;; every class has a unique number <256 assigned to it.  Handy for
;;;;; sending it over the wire.
;;;;;
;;;;; The emphasis was on getting something to work and moving on, so there
;;;;; are a few idiosyncracies to digest.  For example, invoking GFs takes
;;;;; an extra parameter that you wouldn't expect.  Here's a sample call to
;;;;; coco_fire_bullet.  Note the second parameter, which should ALWAYS be
;;;;; exactly, textually, COCO_SOBJECT_CL( first_parameter ):
;;;;;
;;;;;   coco_fire_bullet( sobject, COCO_SOBJECT_CL( sobject ), sstate, target )

;; clisp really wants these structures before all callers.

(defstruct class-info
  name
  (super nil)
  (instance-slots nil)
  (class-slots nil)
  (instance-initforms nil)
  (class-initforms nil)
  (generic-functions nil)   ; only used by root classes
  (methods nil)
  (class-layout :enumerant) 
  (class-name-p nil)        ; only used by root classes
  (class-index-index 0)     ; only used by root classes


  ;; init-by-list-fn can be :none, :virtual, :automatic, or a string.
  
  ;; :none and :virtual behave differently for root classes, they set
  ;; the behavior for all subclasses too, E.g  If root class says :none, 
  ;; then no subclasses may specifiy init-by-list-fn.  Otherwise they
  ;; may specify.
  
  ;; Subclasses with :none have NULL in their init-by-list-fn slot.
  ;; they are effectively unavailable to level-designers.
  
  ;; Subclasses with :virtual inherit their superclasses
  ;; init-by-list-fn.  if no superclass exist, NULL is inherited, and
  ;; the class is effectively unavailable to level-designers.
  
  ;; classes with :automatic have the oo-compiler construct
  ;; init-by-list-fns for them.  These take the same arguments as
  ;; their regular constructors, but in the init-by-list-fn's peculiar
  ;; array-based formal parameters.
  
  ;; classes which specify a string use that string as the name of a
  ;; function to call, which should have the init-by-list-fn's peculiar
  ;; array-based formal parameters.
  (init-by-list-fn :none)
  (init-by-list-count nil);; only must be specified if init-by-list-fn is a string
  )

(defstruct slot-info
  name
  type
  (allocation nil))

(defstruct generic-function-info
  name
  root-class
  return-type
  arg-type-alist
  methods
  no-export-p)

(defstruct method-info
  class 
  function)

(defvar *c-list-element-type* "coco_lnum")

(defvar *c-source-output* t)
(defvar *c-header-output* t)
(defvar *c-consolidated-output* t 
  "Both *c-source-output* and *c-consolidated-output*")
(defvar *c-forward-header-output* t
  "Where to send some extremely forward declarations, that need to be defined before methods can be implemented.")

(defvar *c-source-beginning* (format nil "#include <stdlib.h> /* for rand() */~%#include \"coco.h\"~%#include \"rasters.h\"~%~%"))
(defvar *header-beginning* "")

(defvar *no-edit-warning*
    (format
     nil 
     "/* This file has been automatically generated.  Do not edit it directly. */~%~%"))

(defun emit-everything-to-files (string-pathname-sans-extension)
  "Writes three files that implement the classes requested in the defined environment."
  (with-open-file (*c-forward-header-output*
                   (format nil "forward-~A.h" string-pathname-sans-extension)
                   :direction :output
                   :if-exists :supersede
                   :if-does-not-exist :create)
    (with-open-file (*c-header-output*
                     (format nil "~A.h" string-pathname-sans-extension)
                     :direction :output
                     :if-exists :supersede
                     :if-does-not-exist :create)
      (with-open-file (*c-source-output*
                       (format nil "~A.c" string-pathname-sans-extension)
                       :direction :output
                       :if-exists :supersede
                       :if-does-not-exist :create)
        (let ((*c-consolidated-output* 
               (make-broadcast-stream *c-header-output* *c-source-output*)))
          (format *c-consolidated-output* "~A" *no-edit-warning*)
          (format *c-forward-header-output* "~A" *no-edit-warning*)
          (format *c-header-output* "~A" *header-beginning*)
          (format *c-source-output* "~A" *c-source-beginning*)
          (emit-everything)
          (values))))))

(defun emit-everything ()
  (emit-class-struct-definition)
  (emit-class-struct-variables)
  (emit-instance-definitions)
  (emit-subtype-p)
  (emit-generic-functions)
  (emit-instance-initializers)
  (emit-init-by-list-fn-defaults)
  (emit-accessors)
  (values))

(defparameter *package-prefix* "coco_")

(defparameter *no-method-error*
   "coco_fatal_printf( COCO_ERROR_COCO_IMPLEMENTAION_PROBLEM, \"No method on ~A inst=%p, class_pass_idx = %d, class_slot_idx = %d\\n\", inst, cl ? cl->cl_index : -1, inst->every.cl->cl_index );~%")

(defparameter *classes* nil)

(defparameter *root-classes* nil)

(defmethod print-object ((self class-info) stream)
  (print-unreadable-object (self stream :type t)
    (format stream "~A" (class-info-name self))))

(defmethod print-object ((self slot-info) stream)
  (print-unreadable-object (self stream :type t)
    (format stream "~A" (slot-info-name self))))

(defmacro do-superclasses ((var class-info &optional result) &body body)
  `(let ((,var ,class-info))
     (loop 
       (progn ,@body)
       (setf ,var (class-info-super ,var))
       (when (null ,var) (return ,result)))))

(defun find-class-by-name (class-name)
  (dolist (class-info *classes* nil)
    (when (string= class-name (class-info-name class-info))
      (return class-info))))

(defun find-root-class (class)
  (loop while (class-info-super class)
      do (setf class (class-info-super class))
      finally (return class)))

(defun root-class-p (class)
  (find class *root-classes* :test 'eq))

(defun subclass-p (sub super)
  (eq (find-root-class sub) super))

(defun find-slot-allocation-by-name (name class-info &key no-error-p)
  (assert (class-info-p class-info))
  (dolist (is (class-info-instance-slots class-info))
    (when (string= (slot-info-name is) name)
      (return-from find-slot-allocation-by-name :every-instance)))
  (dolist (is (class-info-class-slots class-info))
    (when (string= (slot-info-name is) name)
      (return-from find-slot-allocation-by-name :every-class)))
  (cond ((class-info-super class-info)
         (find-slot-allocation-by-name name (class-info-super class-info)))
        (no-error-p nil)
        (t (error "Couldn't find anything about a slot named ~S" name))))

(defun find-slot-initform (class slot-name 
                           &optional (allocation :every-instance)
                           &key no-error-p)
  (do-superclasses (super class 
                          (if no-error-p
                              nil
                              (error "Couldn't find initform for ~S"
                                     slot-name)))
    (let ((found (second
                  (find slot-name (if (eq allocation :every-class)
                                      (class-info-class-initforms super)
                                      (class-info-instance-initforms super))
                        :key 'first :test 'string=))))
      (when found
        (return found)))))


(defun lispify-class-name (string)
  (substitute #\- #\_ string))

(defmacro c-define-class (name (&optional superclass) slots 
                          &key (class-layout :enumerant)
                               (named-classes-p nil)
                               (init-by-list-fn :none)
                               (init-by-list-count nil))
  #+ignore ;; UNINTENTIONALLY VESTIGAL?
  (when (null superclass)
    (push (list "super"  (format nil "~A~A_cl *" *package-prefix* name))
          slots))
  (let* ((superclass-info (and superclass (find-class-by-name superclass)))
         (enumerant-p
          (eq :enumerant 
              (if superclass-info
                  (class-info-class-layout superclass-info)
                  class-layout)))) 
    ;; add implicit super pointer
    (push (list* "super" 
                 (if superclass-info
                     (if enumerant-p
                         (format nil "&~A~A_class" *package-prefix* 
                                 (class-info-name superclass-info))
                         (format nil "(~A~A_classes + ~:@(~A~A_CLASS_INDEX~))"
                                 *package-prefix* 
                                 (class-info-name
                                  (find-root-class superclass-info))
                                 *package-prefix*
                                 (class-info-name superclass-info)))
                     "0")
                 (if superclass
                     nil
                     (list* (format nil "~A~A_cl *" *package-prefix* name)
                            '(:allocation :every-class))))
          slots)

    ;; add class-name pointer
    (when (or named-classes-p 
              (and superclass-info
                   (class-info-class-name-p 
                    (find-root-class superclass-info))))
      (push (list* "name" 
                   (format nil "\"~A\"" (lispify-class-name name)) 
                   (if superclass-info
                       nil
                       '("char *" :allocation :every-class)))
            slots))

    ;; add init-by-list-fn pointer
    (when (not (eq :none init-by-list-fn))
      (assert (or init-by-list-count (not (stringp init-by-list-fn))))
      (if superclass-info
          (progn
            (assert (not (eq :none (class-info-init-by-list-fn
                                    (find-root-class superclass-info)))))
            (push (list "init_by_list_count" 
                        "<<<oo compiler error 3>>>") ;; (format nil "~A" init-by-list-count)
                  slots))
          (progn
            (push 
             (list
              "init_by_list_fn" 
              "<<<oo compiler error 1>>>" ;;(if (eq init-by-list-fn :root) "NULL" init-by-list-fn)
              (list (format nil "~A~A *(*"
                            *package-prefix*
                            name)
                    (format nil
                            ")(~A~A *, ~A~A_cl *, ~A *)"
                            *package-prefix* name
                            *package-prefix* name
                            *c-list-element-type*))
              :allocation :every-class)
             slots)
            (push 
             (list "init_by_list_count" 
                   "<<<oo compiler error 2>>>"
                   "int" :allocation :every-class)
             slots))))
    


    ;; add implicit index class slot
    (when (not enumerant-p)
      ;; index
      (push (list* "cl_index"
                   (if superclass-info
                       (format nil "~A"
                               (incf (class-info-class-index-index
                                      (find-root-class superclass-info)))) 
                       "0")
                   (if superclass 
                       nil
                       '("unsigned char"  ;; assumes < 256 classes
                         :allocation :every-class)
                       ))
            slots))

    ;; add implicit class pointer
    (push (list* "cl"
                 :arg
                 (if superclass
                     nil
                     (list (format nil "~A~A_cl *" *package-prefix* name))))
          slots)


    

    (let ((ci (make-class-info
               :name name
               :super superclass-info
               :instance-slots
               (let (i-s)
                 (dolist (slot slots (nreverse i-s))
                   (destructuring-bind 
                       (name initform &optional type &key allocation) slot
                     (declare (ignore initform))
                     (when (and type (not (eq allocation :every-class)))
                       (push (make-slot-info :name name :type type) i-s)))))
               :class-slots
               (let (c-s)
                 (dolist (slot slots (nreverse c-s))
                   (destructuring-bind
                       (name initform &optional type &key allocation) slot
                     (declare (ignore initform))
                     (when (eq allocation :every-class)
                       (assert (null superclass))
                       (push (make-slot-info :name name 
                                             :type type :allocation 
                                             :every-class)
                             c-s)))))
               :class-layout 
               (if enumerant-p
                   :enumerant
                   :index)
               :class-name-p named-classes-p
               :init-by-list-fn init-by-list-fn
               :init-by-list-count init-by-list-count)))
      

      ;; add initializer for class super slot
;; removed because apperantly vestigal
;       (push (list "super" 
;                   (if superclass-info
;                       (if (eq :enumerant 
;                               (class-info-class-layout superclass-info))
;                           (format nil "&~A~A_class" *package-prefix* 
;                                   (class-info-name superclass-info))
;                           (format nil "(~A~A_classes + ~:@(~A~A_CLASS_INDEX~))"
;                                   *package-prefix* 
;                                   (class-info-name
;                                    (find-root-class superclass-info))
;                                   *package-prefix*
;                                   (class-info-name superclass-info)))
;                       "0"))
;           (class-info-class-initforms ci))
    (dolist (slot slots)
      (ecase (find-slot-allocation-by-name (first slot) ci)
        (:every-instance 
         (when (or (stringp (second slot)) (eq :arg (second slot)))
           (push slot (class-info-instance-initforms ci))))
        (:every-class
         (if (stringp (second slot))
             (push slot (class-info-class-initforms ci))
             (error "Every-class slots should always be initialized, but ~A isn't"
                    (first slot))))))
    (setf (class-info-class-initforms ci) 
      (nreverse (class-info-class-initforms ci)))
    (setf (class-info-instance-initforms ci) 
      (nreverse (class-info-instance-initforms ci)))
    #+init-hook
    (when (null superclass-info)
      (let ((gf (make-generic-function-info :name 
                                            (format nil "~A_init_hook"
                                                    (class-info-name ci))
                                            :root-class ci
                                            :return-type "void"
                                            :arg-type-alist nil
                                            :no-export-p t)))
        (push gf (class-info-generic-functions ci))
        (push (make-method-info :class ci :function
                                (format nil "~A~A_init_hook_noop"
                                        *package-prefix* (class-info-name ci)))
         (generic-function-info-methods gf))))
    (when (null superclass-info)
      (push ci *root-classes*))
    (push ci *classes*)
    nil)))

(defun format-variable-declaration (stream slot 
                                    &optional (prefix "") (suffix "")
                                              newline-p)
  (if (consp (slot-info-type slot))
      (format stream (if newline-p "~A~A ~A ~A~A~%" "~A~A ~A ~A~A") prefix 
              (first (slot-info-type slot))
              (slot-info-name slot)
              (second (slot-info-type slot))
              suffix)
      (format stream (if newline-p "~A~A ~A~A~%" "~A~A ~A~A") prefix 
              (slot-info-type slot)
              (slot-info-name slot)
              suffix)))

(defun format-type-declaration (stream slot &optional (prefix "") (suffix "")
                                                      newline-p)
  (if (consp (slot-info-type slot))
      (format stream (if newline-p "~A~A ~A~A~%" "~A~A ~A~A") prefix 
              (first (slot-info-type slot))
              (second (slot-info-type slot))
              suffix)
      (format stream (if newline-p "~A~A~A~%" "~A~A~A") prefix 
              (slot-info-type slot)
              suffix)))

(defun emit-class-struct-definition ()
  (format *c-header-output* "~&/* Class structure definition */~%~%")
  (format *c-forward-header-output* "~&/* Class structure definition */~%~%")
  (dolist (class *root-classes*)
    (format *c-forward-header-output* "~&typedef struct ~A~A_class_ ~A~A_cl;~%"
            *package-prefix* (class-info-name class)
            *package-prefix* (class-info-name class))
    (format *c-forward-header-output* "~&union ~A~A_inst;~%" *package-prefix*
            (class-info-name class))
    (format *c-forward-header-output* "typedef union ~A~A_inst ~A~A;~%~%"
            *package-prefix* (class-info-name class)
            *package-prefix* (class-info-name class))
    (format *c-header-output* "struct ~A~A_class_ {~%" *package-prefix*
            (class-info-name class))
    (dolist (slot (class-info-class-slots class))
      ;; was: (format *c-header-output* "  ~A ~A;~%" (slot-info-type slot) (slot-info-name slot))
      (format-variable-declaration *c-header-output* slot "  " ";" t)
      )
    (dolist (gf (class-info-generic-functions class))
      (format *c-header-output* "  ")
      (generic-function-info-format-function-var 
       gf (generic-function-info-name gf) *c-header-output*)
      (format *c-header-output* ";~%"))
    (format *c-header-output* "};~%~%")))

(defun emit-class-struct-variables ()
  (format *c-consolidated-output* "~&/* Class structures themselves */~%~%")
  (dolist (rclass (reverse *root-classes*))
    ;; emit default init_hook method, if needed.
    #+init-hook
    (let ((gfname (format nil "~A_init_hook" (class-info-name rclass))))
      (dolist (gf (class-info-generic-functions rclass))
        (when (string= gfname (generic-function-info-name gf))
          (let ((noop-name (format nil "~A~A_init_hook_noop"
                                   *package-prefix*
                                   (class-info-name rclass))))
            (dolist (method (generic-function-info-methods gf))
              (when (string= noop-name (method-info-function method))
                (format *c-source-output*
                        "static void ~A( ~A~A *inst, ~A~A_cl *cl )~%{~%  return; /* default method does nothing */~%}~%"
                        noop-name *package-prefix* 
                        (class-info-name rclass) *package-prefix*
                        (class-info-name rclass)))
              (when (eq (method-info-class method) rclass)
                ;; it is shadowed by a (presumably user defined) method
                (return))))
          (return t))))
    (labels ((format-class-initializers (class root-class tab-p)
               (dolist (slot (class-info-class-slots root-class))
                 (let ((initform 
                        (find-slot-initform class (slot-info-name slot)
                                            :every-class :no-error-p t)))
                   (cond 
                    ((string= (slot-info-name slot) "init_by_list_count")
                     (if (null (class-info-init-by-list-count class))
                         (let ((count -1))
                           (map-inherited-instance-slots 
                            (lambda (slot)
                              (when (eq :arg
                                        (find-slot-initform 
                                         class (slot-info-name slot) 
                                         :every-instance :no-error-p t))
                                (incf count)))
                            class)
                           (setf initform (format nil "~A" count)))
                         (setf initform 
                           (format nil "~A"
                                   (class-info-init-by-list-count class)))))
                    ((string= (slot-info-name slot)
                              "init_by_list_fn")
                     (let* ((tclass class)
                            (initer
                             (loop 
                                 while tclass
                                 when (not (eq :virtual 
                                               (class-info-init-by-list-fn
                                                tclass)))
                                 return 
                                   (class-info-init-by-list-fn tclass)
                                 do (setf tclass
                                      (class-info-super tclass)))))
                       (setf initform
                         (case initer
                           ((nil) "NULL")
                           (:none "NULL")
                           (:virtual (error "This shouldn't be happening"))
                           (:automatic
                            (format nil "~A~A_automatic_init_by_list"
                                    *package-prefix*
                                    (class-info-name tclass)))
                           (t (assert (string initer))
                              initer)))))
                    ((null initform)
                     (error "Missing initform for ~A's ~A slot"
                            (class-info-name class)
                            (slot-info-name slot)))
                    (t nil))
                   (format *c-source-output*
                           "  ~A~A,~40T/* ~A */~%"
                           (if tab-p "  " "")
                           initform
                           (slot-info-name slot))))
               (dolist (gf (class-info-generic-functions root-class))
                 (let ((method (find class
                                     (generic-function-info-methods gf)
                                     :test #'eq :key 'method-info-class)))
                   (format *c-source-output* "  ~A~A,~%" 
                           (if tab-p "  " "")
                           (if method (method-info-function method) 0))))))
      (ecase (class-info-class-layout rclass)
        (:enumerant
         (dolist (class (reverse *classes*))
           (when (subclass-p class rclass)
             (format *c-header-output* "~&extern ~A~A_cl ~A~A_class;~%"
                     *package-prefix* (class-info-name rclass)
                     *package-prefix* (class-info-name class))
             (format *c-source-output* "~&~A~A_cl ~A~A_class = {~%"
                     *package-prefix* (class-info-name rclass)
                     *package-prefix* (class-info-name class))
             (format-class-initializers class rclass nil)
             (format *c-source-output* "};~%~%"))))
        (:index
         (format *c-header-output* "extern ~A~A_cl ~A~A_classes[];~%"
                 *package-prefix* (class-info-name rclass)
                 *package-prefix* (class-info-name rclass))
         (format *c-source-output* "~A~A_cl ~A~A_classes[] = {~%"
                 *package-prefix* (class-info-name rclass)
                 *package-prefix* (class-info-name rclass))
         (let ((class-count 0))
           (dolist (class (reverse *classes*))
             (when (subclass-p class rclass)
               (incf class-count)
               ;; source
               (format *c-source-output* "  { /* ~A~A */~%"
                       *package-prefix* (class-info-name class))
               (format-class-initializers class rclass t)
               (format *c-source-output* "  },~%")
               ;; header
               (format *c-header-output*
                       "#define ~:@(~A~A_CLASS_INDEX~) ~A~%"
                       *package-prefix* (class-info-name class) 
                       (find-slot-initform class "cl_index" :every-class))))
           (format *c-header-output*
                   "#define ~:@(~A~A_CLASS_COUNT~) ~A~%~%"
                   *package-prefix* (class-info-name rclass) 
                   class-count))
         (format *c-header-output* "~%")
         (dolist (class (reverse *classes*))
           (when (subclass-p class rclass)
             ;; header
             (format *c-header-output*
                     "#define ~:@(~A~A_CLASS~) (~A~A_classes + ~:@(~A~A~)_CLASS_INDEX)~%"
                     *package-prefix* (class-info-name class)
                     *package-prefix* (class-info-name rclass)
                     *package-prefix* (class-info-name class))))
         (format *c-source-output* "};~%")))))
  (format *c-header-output* "~%"))


(defun emit-init-by-list-fn-defaults ()
  (format *c-consolidated-output* "~&/* Init-by-list function defaults */~%~%")
  (dolist (rclass (reverse *root-classes*))
    (when (class-info-init-by-list-fn rclass)
      (dolist (class (reverse *classes*))
        (when (and (eq :automatic (class-info-init-by-list-fn class))
                   (subclass-p class rclass))
          (format *c-consolidated-output*
                  "~A~A *~A~A_automatic_init_by_list( ~A~A *inst, ~A~A_cl *cla, ~A *arguments )"
                  *package-prefix*
                  (class-info-name rclass)
                  *package-prefix*
                  (class-info-name class)
                  *package-prefix*
                  (class-info-name rclass)
                  *package-prefix*
                  (class-info-name rclass)
                  *c-list-element-type*)
          (format *c-header-output* ";~%")
          (format *c-source-output* "~%")
          (format *c-source-output* "{~%")
          (format *c-source-output* "  return ~Ainit_~A( inst, cla"
                  *package-prefix* (class-info-name class))
          (let ((count -1))
            (map-inherited-instance-slots 
             (lambda (slot) 
               (when (eq :arg
                         (find-slot-initform class (slot-info-name slot) 
                                             :every-instance :no-error-p t))
                 (incf count))) 
             class)
            (dotimes (i count)
              (format *c-source-output* ", arguments[~A]" i))
            (format *c-source-output* " );~%"))
          (format *c-source-output* "}~%"))))))

(defmethod print-object ((self generic-function-info) stream)
  (print-unreadable-object (self stream :type t)
    (format stream "~A" (generic-function-info-name self))))

(defmacro c-define-generic-function (name root-class-name return-type
                                     arg-type-alist)
  (let ((root-class (find-class-by-name root-class-name)))
    (assert (root-class-p root-class))
    (push (make-generic-function-info :name name :root-class root-class
                                      :arg-type-alist arg-type-alist
                                      :return-type return-type)
          (class-info-generic-functions root-class))
    nil))

(defun find-generic-function (name some-class)
  (let ((root-class (find-root-class some-class)))
    (or (find name (class-info-generic-functions root-class)
              :test 'string=
              :key 'generic-function-info-name)
        (error "Couldn't find a generic function named ~S" name))))

(defmethod print-object ((self method-info) stream)
  (print-unreadable-object (self stream :type t)
    (format stream "~A" (method-info-function self))))

(defmacro c-define-method (gf-name class-name implementation-function)
  (let ((class (find-class-by-name class-name)))
    (assert class)
    (push (make-method-info :class class :function implementation-function)
          (generic-function-info-methods
           (find-generic-function gf-name class)))
    nil))

(defun map-superclasses-reverse (function class)
  (when class
    (map-superclasses-reverse function (class-info-super class))
    (funcall function class)))

(defun map-inherited-instance-slots (function class)
  "Map function over all the instance slots in function, in order of appearance in the implementation structure."
  (map-superclasses-reverse 
   (lambda (class)
     (dolist (slot (class-info-instance-slots class))
       (funcall function slot)))
   class))

(defun emit-instance-definitions ()
  (format *c-header-output* "~&/* Instance Definitions */~%~%")
  (dolist (class *classes*)
    (format *c-header-output* "~&struct ~A~A~A {~%" *package-prefix* 
            (if (root-class-p class) "every_" "")
            (class-info-name class))
    (map-inherited-instance-slots 
     (lambda (slot)
       ;; was: (format *c-header-output* "  ~A ~A;~%" (slot-info-type slot) (slot-info-name slot))
       (format-variable-declaration *c-header-output* slot "  " ";" t))
     class)
    (format *c-header-output* "};~%~%"))
  (dolist (class *root-classes*)
    (format *c-header-output* "union ~A~A_inst {~%" *package-prefix* (class-info-name class))
    (dolist (subclass *classes*)
      (when (subclass-p subclass class)
        ;;(note subclass)
        (let ((root-p (root-class-p subclass)))
          (format *c-header-output* "  struct ~A~A~A ~A;~%"
                  *package-prefix*
                  (if root-p "every_" "")
                  (class-info-name subclass)
                  (if root-p "every" (class-info-name subclass))))))
    (format *c-header-output* "};~%"))
  (format *c-header-output* "~%"))

(defun emit-subtype-p ()
  (dolist (class *root-classes*)
    (let ((ciname (class-info-name class)))
      (format *c-consolidated-output*
              "~&int ~A~A_subtype_p( ~A~A_cl *sub, ~A~A_cl *super )"
              *package-prefix* ciname
              *package-prefix* ciname 
              *package-prefix* ciname)
      (format *c-header-output* ";~%")
      (format *c-source-output* "~%{~%")
      (format *c-source-output* "  do~%")
      (format *c-source-output* "    if (sub==super) return 1;~%")
      (format *c-source-output* "    else sub = sub->super;~%")
      (format *c-source-output* "  while (sub);~%")
      (format *c-source-output* "  return 0;~%")
      (format *c-source-output* "}~%"))))

(defun emit-generic-functions ()
  (format *c-consolidated-output* "~&/* Generic Functions */~%~%")
  (dolist (class *root-classes*)
    (dolist (gf (class-info-generic-functions class))
      (let* ((no-export-p (generic-function-info-no-export-p gf))
             (proto-stream 
              (if no-export-p *c-source-output* *c-consolidated-output*)))
        (format proto-stream
                "~&~A~%~A~A( ~A~A * inst, ~A~A_cl * cl"
                (generic-function-info-return-type gf)
                *package-prefix*
                (generic-function-info-name gf)
                *package-prefix*
                (class-info-name class)
                *package-prefix*
                (class-info-name class))
        (loop for (name type) in
              (generic-function-info-arg-type-alist gf)
            do (format proto-stream ", ~A ~A" type name))
        (when (not no-export-p)
          (format *c-header-output* " );~%"))
        (format *c-source-output* " )~%{~%")
        (format *c-source-output* "  do~%")
        (format *c-source-output* "   {~%")
        (format *c-source-output* "     ")
        (generic-function-info-format-function-var gf "method" *c-source-output*)
        (format *c-source-output* " = cl->~A;~%"
                (generic-function-info-name gf))
        (format *c-source-output* "     if (method)~%")
        (progn
          (format *c-source-output* "       ~Amethod( inst, cl"
                  (if (string= "void" (generic-function-info-return-type gf))
                      "{ "
                      "return "))
          (loop for (name nil) in
                (generic-function-info-arg-type-alist gf)
              do (format *c-source-output* ", ~A" name))
          (format *c-source-output* " );~A~%"
                  (if (string= "void" (generic-function-info-return-type gf))
                      " return; }"
                      "")))
        (format *c-source-output* "     cl = cl->super;~%")
        (format *c-source-output* "   }~%")
        (format *c-source-output* "  while (cl);~%  ")
        (format *c-source-output* *no-method-error* 
                (generic-function-info-name gf) )
        (if (string= "void" (generic-function-info-return-type gf))
            (format *c-source-output* "  return; /* convince compiler we don't fall off end */~%")
            (format *c-source-output* "  return (~A) 0; /* convince compiler we don't fall off end */~%"
                    (generic-function-info-return-type gf)))
        (format *c-source-output* "}~%~%"))))
  (format *c-header-output* "~%"))


(defun generic-function-info-format-function-var (gf var-string stream)
  (let ((root-class (generic-function-info-root-class gf)))
    (format stream "~A (* ~A)( ~A~A *, ~A~A_cl * "
            (generic-function-info-return-type gf)
            var-string
            *package-prefix*
            (class-info-name root-class)
            *package-prefix*
            (class-info-name root-class))
  (loop for (nil type) in (generic-function-info-arg-type-alist gf)
      do (format stream ", ~A " type))
  (format stream ")")))

(defun emit-instance-initializers ()
  (format *c-consolidated-output* "~&/* Instance Initializers */~%~%")
  (dolist (class (reverse *classes*))
    (let ((rclass (find-root-class class))
          (super (class-info-super class)))
      (format *c-consolidated-output* "~&~A~A *~%"
              *package-prefix* (class-info-name rclass))
      (format *c-consolidated-output* "~Ainit_~A( ~A~A * inst"
              *package-prefix*
              (class-info-name class)
              *package-prefix* (class-info-name rclass))
      (map-inherited-instance-slots 
       (lambda (slot)
         (when (eq :arg (find-slot-initform class (slot-info-name slot) 
                                            :every-instance :no-error-p t))
           ;; was: (format *c-consolidated-output* ", ~A ~A" (slot-info-type slot)
           ;; was:         (slot-info-name slot))
           (format-variable-declaration *c-consolidated-output* slot ", ")
           ))
       class)
      (format *c-header-output* " );~%")
      (format *c-source-output* " )~%{~%")
      ;; call our super's initializer
      (when super
        (format *c-source-output* "  ~Ainit_~A( inst"
                *package-prefix*
                (class-info-name super))
        (map-inherited-instance-slots 
         (lambda (slot)
           (when (eq :arg (find-slot-initform super (slot-info-name slot) 
                                              :every-instance :no-error-p t))
             (let ((this-initform
                    (find-slot-initform
                     class (slot-info-name slot)
                     :every-instance :no-error-p t)))
               (format 
                *c-source-output* ", ~A" 
                (if (eq :arg this-initform)
                    (slot-info-name slot)
                    (or this-initform
                        ;; was: (format nil "((~A)0)" (slot-info-type slot))
                        (format-type-declaration nil slot "((" ")0)")))))))
         super)
        (format *c-source-output* " );~%"))
      ;; now initialize our slots (that were'nt already just init'ed)
      (loop for (name initform) in (class-info-instance-initforms class) do
            (let ((super-initform
                   (and super
                        (find-slot-initform super name 
                                            :every-instance :no-error-p t))))
              (when (and initform 
                         (not (eq super-initform :arg)))
                (format *c-source-output* "  inst->~A.~A = ~A;~%"
                        (if super (class-info-name class) "every")
                        name
                        (if (eq initform :arg) 
                            name
                            initform)))))
      (format *c-source-output* "  return inst;~%")
      (format *c-source-output* "}~%~%")))
  (format *c-header-output* "~%"))

(defun emit-accessors ()
  (format *c-header-output* "~&/* Accessors */~%~%")
  (dolist (class *classes*)
    (dolist (slot (class-info-instance-slots class))
      (format *c-header-output* "#define ~:@(~A~A_~A~)( clss ) ((clss)->~A.~A)~%"
              *package-prefix*
              (class-info-name class)
              (slot-info-name slot)
              (if (root-class-p class) "every" (class-info-name class))
              (slot-info-name slot)))
    (dolist (slot (class-info-class-slots class))
      (format *c-header-output*
              "#define ~:@(~A~A_~A~)( clss ) ((clss)->every.cl->~A)~%"
              *package-prefix*
              (class-info-name class)
              (slot-info-name slot)
              (slot-info-name slot))))
  (format *c-header-output* "~%"))


#+ignore
(progn
(c-define-class "sobject" ()
                (("x" "0" "int")
                 ("y" "0" "int")
                 ("dx_max" "0" "int" :allocation :every-class)
                 ("dy_max" "0" "int" :allocation :every-class)))

(c-define-class "copter" ("sobject")
                (("bullets" "30" "unsigned int")
                 ("dx_max" "100")
                 ("x" "400")))

(c-define-class "building" ("sobject")
                (("x" :arg)
                 ("y" "COCO_GROUND_Y")))

(c-define-class "building_x100" ("building")
                (("x" "100")
                 ("y" "COCO_GROUND_Y")))


(c-define-generic-function "draw" "sobject" "void" (("x" "int")))

(c-define-method "draw" "copter" "coco_draw_copter")
)
