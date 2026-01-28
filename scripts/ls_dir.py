#!/usr/bin/env python3
import os

#Config
ROOT_DIR="c:/John_Wood/Coding_Projects/c/KVDB"
DIR_PATH_RELATIVE="res/images"
DIR_PATH=ROOT_DIR+'/'+DIR_PATH_RELATIVE
EXPORT_PATH=ROOT_DIR+"/tmp"
TXT_FILE_PATH=EXPORT_PATH+"/dir_entries.txt"
#EndConfig

def main():
	os.system(f"echo. > {TXT_FILE_PATH}")
	f_txt = open(TXT_FILE_PATH, "w")
	print("dir \"" + DIR_PATH_RELATIVE + "\":")
	f_txt.write("const char* filenames[] = {\n")
	dir_ls = os.listdir(DIR_PATH)
	dir_entry_count = len(dir_ls)
	for i in range(0, dir_entry_count):
		filename = dir_ls[i]
		f_txt.write("\t")
		f_txt.write('\"'+ DIR_PATH_RELATIVE + '/' + filename + '\"')
		if (i < dir_entry_count - 1):
			f_txt.write(",\n")
		else:
			f_txt.write("\n")
		#endif
		print("\x1b[32m" f"\t{filename}" "\x1b[0m")
	#endfor
	f_txt.write("};\n")
	f_txt.write(f"\n#define ENTRY_COUNT {dir_entry_count}\n")
	f_txt.close()
#endef

if (__name__ == "__main__"):
	main()
#endif
