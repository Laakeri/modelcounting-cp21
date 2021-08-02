To compile:

make


To use with c2d:

./vtree -decot 900 -out_cnf out.cnf -out_dtree out.dt instance.cnf
./c2d -in_memory -cache_size 16000 -smooth_all -count -dt_in out.dt -in out.cnf


To use with minic2d:

./vtree -minic2d -decot 900 -out_cnf out.cnf -out_dtree out.dt instance.cnf
./miniC2D -C -i -s 16000 --vtree out.dt --cnf out.cnf