To launch the kernel: 
- create a VM in virtual box with the name "My Micro Kernel".
- create the ISO file using: make microkernel.iso
- Make the VM point to the .iso file to boot from it.
- launch using: make run

Note: There are 3 flavors. The default is flavor1. To launch one of the others, include the name of the flavor in the make command as the following: "make run flavor=flavor<number of the kernel flavor>".

Another Note: Before compiling any flavor, be sure to "make clean" to get rid of any previous left overs form other flavors.
