This program is to demo how to create a structure/enum within Ghidra.
structures is the binary to reverse the struct student.
solcheck will print the offsets and size of the struct student for students to check their work.

See structures.c for source code:
- creates s_STUDENT structure on the stack
- Note that s_STUDENT's last member is an array of char[32]
- s_STUDENT contains age, gender, class, grade, and name members
- both amanda and bob are s_STUDENT struct instances
- s_amanda instance of s_STUDENT is passed as pointer within Stuff() to GetAmanda() as local_48
- s_bob instance of s_STUDENT is populated within Stuff() as local_88

Ghidra steps:
- make the binary 'structures' and open with Ghidra
- see bottom-left 'Data Types Manager' window - be sure the pointer finger is on (doesn't have a red slash)
- right click on 'structures' and select 'new-->structure'
- name/typedef your structure (s_STUDENT)
- add 5 params based on what you see within the listing/decompiler views within the Stuff() or PrintStudent() fuctions.  I like PrintStudent() as it clearly shows the base(structure), member names, data widths/sizes, and offset/order

Member details (Offset, Length, ..., Data Type, Name, ...):
Note: start with Data Type and the rest of the fields auto populate - then add the name.  
	0, 8, long, 'age'
	8, 1, char, 'gender'
	16,8, long, 'grade'
	24, 4, dword, 'class' -- note this is actually an Enum
	28,8, char*,'name' -- this is a guess from the format string using %s. As we see later, this really should be a char array and not a char *.
Note: 'Size' (at the bottom) should == 36 at this point with 1 as alignment and 'Align' unchecked.

- click the 'pack' box (bottom right) and note that 'Size' (bottom left) changed to 40
- unclick the 'pack' box and note that the 'gender' and 'class' members now shows additional undefined bytes (used to pad your sturcture) 
Note: Ghidra lists the offset to the 'name' member as 0x1c or 28 bytes - not the 32nd byte as expressed in our current struct configuration. 
Note: might also see the 'name' member referenced as 'struct.class + 4'
Note: might also see the 'grade' member referenced as 'struct.gender + 7'
TO FIX: move the 'char* name' member to the 28th byte offset i.e. directly after the 'class' member
Note: leave the 7 undefined bytes as part of the 'gender' member

The step above will define and save s_STUDENT in your 'structures' binary's data type manager within Ghidra
Note: you will also need to create a s_STUDENT* (pointer) to this new struct
- right click on s_STUDENT within the 'structures' dropdown in the data type managers window in Ghidra
- click 'new->pointer to structure' -- name it and save it - should now be available for assignment

To retype variables:
- within Ghirda->Stuff() right click on the variable (local_88) that represents the s_STUDENT Bob and 'Retype Variable' to STUDENT
- In the Data Type Chooser Dialog box, type in the name of your newly created structure or struct pointer. 
Note: repeat for local_48 (Amanda)
- click the desired option, select 'yes' if a message box arrears notifiying you that all other references will be changed, etc...
- the variable should now be your struct/struct* and should have auto populated the member names/values. 

-Navigate to GetAmanda
-Note that the decompiler view is still not cleanly showing param_1->field for a couple more fields.
-You may see param_1[1].age =0, &param_1[1].gender=0, param_1[1].grade=0. Fields like these indicate that the structure is not quite right.
-Looking at the listing view, notice that the fields right after name are being zeroed out.
-This is an example of an optimized,inlined strncpy similar to the compiler example.
-The rest of the bytes in the buffer are set to NULL up to count characters.
-The furthest out we get is setting a qword at offset 0x34 to 0, which is 60. 60(last byte in name)-28(name's offset in s_student)=32 is the size of the char array
-Retype name from char* to a char[32], making the size of the s_student 60 bytes.

-Navigate back to Stuff()
-Retype the arguments to PrintStudent and GetAmanda to struct s_student.
-All parameters should be cleaned up
-Note that local88 and local48 (the two structs) are 0x40 away from each other, which is 64 bytes not 60 bytes.
-This is again because padding from struct packing. Check and uncheck the "pack" checkbox like earlier to get 4 "undefined" members of padding after name, yielding the packed size of the struct as 64 bytes.
-Note that changing the size of the struct from 60 to 64 (after retyping the parameters) does not yield different decompiler output.
-Test your structure against the solution file sol to see the offsets and stucture size

Repeat this exact process for creating the Enum (not pointer) passed to get_class_str().  
