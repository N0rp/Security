cd ../not_infected
bash compile_infectable_files.bash
cd ../infected
echo "----------1st------"
./virus
#echo "----------2nd------"
#./virus
#diff echo_infectable.o echo_infectable.o.copy
#objdump -p echo_infectable.o
echo "------------------------"
echo "Executing infected file:"
./echo_infectable.o foo
echo "Executing not infected file:"
./echo_not_infectable.o foo
