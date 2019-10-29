dmreplacefile --SRCFILE="ModuleImport.cmake" --TPLFILE="ModuleImport.cmake" --PATH="..\thirdparty"
dmreplacefile --SRCFILE="ModuleCompileOptions.cmake" --TPLFILE="ModuleCompileOptions.cmake" --PATH="..\thirdparty"

dmreplacefile --SRCFILE="ModuleImport.cmake" --TPLFILE="ModuleImport.cmake" --PATH="..\src"
dmreplacefile --SRCFILE="ModuleCompileOptions.cmake" --TPLFILE="ModuleCompileOptions.cmake" --PATH="..\src"
PAUSE