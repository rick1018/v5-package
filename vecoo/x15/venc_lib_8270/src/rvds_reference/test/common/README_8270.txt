*******************************************************************************
*******************************************************************************
*********** This is a step by step guide to 8270 encoder testing!  ************
*******************************************************************************
*******************************************************************************

1. Checking out, configuring and compiling SW.

1.1 Checking SW out from version control.

    -You can see all the tags in the main tree with command
        
	->git tag
	
    -First you have to clone the working tree from the git main repository:
    
        ->git clone -n /afs/hantro.com/projects/8270/git/8270_encoder
	
    -Then you need to check out the correct tag and make a branch of it	
	
	->cd 8270_encoder
	->git checkout sys8270_x_x 
	->git checkout -b x_x    
    
    -Normally you get Software versions from tag mails you get
     from SW guys (SaPi). Tags can also be found from 
     Sharepoint at 8270 project site

1.2 Configuring and compiling SW

    -Go to 8270_encoder/ and make a test directory:
     
        -> mkdir testdir
     
    -Copy the main scripts to the test directory
    
        ->cp -r software/linux_reference/test/common/masterscripts/ testdir
	
    -Go to the masterscripts folder	
     
    -Open commonconfig.sh and edit the following lines:
    
        ->export swtag="enc8270_x_x"        #This is the tag with which you checked the software out from version control
        
	->export hwtag="enc8270_x_x"        #This you usually get from tag mails, otherwise ask Ari Hautala
	
	->export systag="enc8270_x_x"     #This is the system model which corresponds to the software tag, you can check
	                                   #it from /afs/hantro.com/projects/8270/taghistory/. Also verify that the
				           #system model in /export/work/testdata/sysmodels/8270v/8270_encoder is this one
				           #and that the test data in /misc/export/work/testdata/8270_testdata is created 
				           #with this model.
	
	->export kernelversion=/2.6.24-arm2-spnlck/v0_1/	#Set which linux kernel was used for testing
	
	->export rootfsversion=v2_3_0_2				#Root file system
	
	->export compilerversion=arm-2007q1-21-arm-none-linux-gnueabi-i686-pc-linux-gnu #Set which compiler was used to compile
											#software

	->export testdeviceip=72	   #Set IP for test device
	
	->export testdeviceversion=v0_1

	->export comment=""		   #Set if there is something special in testing
	
	->BOARD="VERSATILE"		   #This is the board with which you run the tests, the last two numbers of
	                                   #its IP show which board it is: 
					   #    4*:  VERSATILE (AHB-bus)
					   #    7*:  EB (Emulation Board, AXI-bus)
					   #    8*:  AXIVERSATILE (AXI-bus)
        
	->TEST_DEVICE_IP="44"
	
	->REMOVE_HANTRO_USER_DATA="y"	   #Use y for testing releases for Sony. Otherwise use n.
	
	->COMPILER_SETTINGS=""             #Board specific value, this can be seen from board status monitor: http://192.168.30.104/monitor/index.php
	                                  
	->KDIR=""                          #Board specific value, this can be seen from board status monitor: http://192.168.30.104/monitor/index.php
	
	->DWL_IMPLEMENTATION="POLLING"     #Then you have to decide do you use IRQ or POLLING mode. IRQ's do not
                                           #work with all the boards yet, but normally they are recommended.
                                           #AXI versatile boards do not support IRQs yet so with 
                                           #those you have to use polling mode.				
		
	->INTERNAL_TEST="y"		   #With this flag you can either enable or disable internal
				           #test cases from tests. Should be n with customer
					   #releases, otherwise y
					
	->MAX_WIDTH=""			   #You can see from the hardware tag, which resolution 
        ->MAX_HEIGHT=""                    #this configuration supports.				
				  				
	->SYSTEM_MODEL_HOME=""		   #The directory where the system model is				
					
	->test_case_list_dir=""		   #The directory where test_data_parameter_x.sh files are.
	
	->test_data_home=""		   #The directory where the test data is
	
	->csv_path=""			   #The directory where test reports are generated.				
					
    -Run script ./set.sh in test directory. 
    
    -This script cleans the test directory, changes all the necessary software
     parameters automatically, compiles the software and test benches, and copies all the test scripts and binaries
     to the test directory.			
			

2 Laboratory testing.


    -First you have to log in to test board from xterm.
    >telnet 192.168.30.xx
    >username -> root 				
    
    #verify that the tag corresponds to the
    #one in RTL, this can be checked from the hw base address when logged on to
    the board: 
            -dm2 0xC0000000 (Versatile board), 
	         0xC4000000 (Axi Versatile board) and 
	         0x84000000 (Emulation Board)
				
    Go to the testdir/masterscripts and run script

    >./runall.sh >& run.log

    -This script starts by loading the memalloc and driver modules to the board. 
    
        -If something goes wrong probable reason is wrong kernel, wrong base address or wrong memory start address. 
         Return value -1 normally means wrong kernel
        -You can check the board's kernel from http://192.168.30.104/monitor/, so that it is the same you defined for the sw. 
         Otherwise TePe can support with this. 
    
    -Then it runs a small test set to ensure
    -that the test environment is set up correctly. You have to open a new shell and log into Matti's computer
    >ssh linuxws27
    
    -Run the script
    
    >./checkall.sh 
    
    to check the results of the small test set.
    
    -If everything goes ok, just wait for the tests to finish, this usually lasts until the next day.
    
    -If something goes wrong, check the file commonconfig.sh and start again from running set.sh.

    
