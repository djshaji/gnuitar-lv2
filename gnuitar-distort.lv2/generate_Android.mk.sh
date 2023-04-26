for i in *c ; 
do 
	FILE=$i   ;  
	MODULE="`basename $PWD`"
	echo -e  LOCAL_PATH := \$\(call my-dir\) \\r\\ninclude \$\(CLEAR_VARS\)\ \\nLOCAL_MODULE := $MODULE \\nLOCAL_SRC_FILES := $i \\ninclude \$\(BUILD_SHARED_LIBRARY\) \\nLOCAL_C_INCLUDES := dsp/ plugin/ > jni/Android.mk ; 
	ndk-build ; 
done 
