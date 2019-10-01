# SpaceRun2 for Linux
SpaceRun2のソースコードをLinux向けに書き換えたものです。(主にOpenSiv3D部)
ドキュメントなどに従ってOpenSiv3Dをコンパイルしたあと、生成されたlibSiv3D.aを~/.local/Siv3D/libに、
OpenSiv3D/Siv3D/include/*を~/local/Siv3D/includeに移動させ、cmake→makeでコンパイルしてください。
実行ファイルの出来た場所にfor linux/resourcesを移動させ、実行ファイルを起動することで起動することが出来ます。
