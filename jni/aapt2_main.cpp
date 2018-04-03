#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <libgen.h>
#include <sys/system_properties.h>
#include <jni.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>

#define DEBUG_TAG "SubsAapt"
#include "com_android_tools_aapt2_Aapt2Jni.h"

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

#include "android-base/logging.h"
#include "ScopedUtfChars.h"

#include "Diagnostics.h"
#include "util/Util.h"

using android::StringPiece;

extern int MainImpl(int argc, char** argv);

jint Java_projekt_substratum_compiler_aapt2_Aapt2_aapt2_main(JNIEnv *env, jobject , jstring args) {
	jboolean isCopy;
	const char *sz = env->GetStringUTFChars(args, JNI_FALSE);
	char *ptr1, *ptr2;
	int i, idx, argc=1, len;

	__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "Native method call: aapt_jni (%s)", sz);
	len= static_cast<int>(strlen(sz));
	for (i=0; i<len; i++) if (sz[i]=='\t') argc++;
	char * argv[argc];
	ptr1 = ptr2 = (char*) sz;
	idx = 0;
	for (i=0; i<len; i++)
	{
		if (*ptr2=='\t')
		{
			*ptr2=0;
			argv[idx]=ptr1;
			idx++;
			ptr1=ptr2+1;
		}
		ptr2++;
	} // for
	argv[idx]=ptr1;

	// redirect stderr and stdout
	freopen ("/sdcard/.substratum/native_stderr.txt", "w", stderr);
	freopen ("/sdcard/.substratum/native_stdout.txt", "w", stdout);

	fprintf (stdout, "Aapt arguments:\n");
	for (i=1; i<argc; i++) fprintf (stdout, "%s\n",argv[i]);

	// call aapt
	jint rc = MainImpl(argc, argv);

	// stopping the redirection
	fclose (stderr);
	fclose (stdout);

	return rc;
}

/*
int doit(int argc, char **argv) {
	printf(" [+] Initializing JavaVM Instance\n");
	JavaVM *vm = NULL;
	JNIEnv *env = NULL;
	jobject assetManager = NULL;
	jobject obj;
	JavaVM *jvm_ = vm;
	JNIEnv *jenv_;

	int status = jvm_->GetEnv((void **) &env, JNI_VERSION_1_6);
	AAssetManager *mgr = AAssetManager_fromJava(env, assetManager);
	AAsset *asset = AAssetManager_open(mgr, (const char *) "test.txt", AASSET_MODE_UNKNOWN);
	long size = AAsset_getLength(asset);
	char *buffer = (char *) malloc(sizeof(char) * size);

	if (status == 0) {
		printf(" [+] Initialization success (vm=%p, env=%p)\n", vm, env);
	} else {
		printf(" [!] Initialization failure (%i)\n", status);
		return -1;
	}

	printf(" [+] Calling JNI_OnLoad\n");

	if (NULL == asset) {
		__android_log_print(ANDROID_LOG_ERROR, "AssetTest", "");
	}

	AAsset_read(asset, buffer, static_cast<size_t>(size));
	__android_log_print(ANDROID_LOG_ERROR, "AssetTest", "");
	AAsset_close(asset);
	return 0;
}
 */
