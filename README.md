# pokemonYellowGambling

![game_window](https://github.com/Clayton-Klemm/Pokemon-Yellow-Gambling-With-OpenCV-Template-Matching/assets/36232582/a6b19e65-681a-477b-af18-ebf491f0a85f)

This project uses OpenCV so make sure you install it. https://opencv.org/get-started/
Once you have it extracted you'll need to configure visual studio to link the dll files in order to use OpenCV

![right_click_project_properties](https://github.com/Clayton-Klemm/Pokemon-Yellow-Gambling-With-OpenCV-Template-Matching/assets/36232582/9905db2a-a131-4cba-bbcc-87617447c7b1)
![config1](https://github.com/Clayton-Klemm/Pokemon-Yellow-Gambling-With-OpenCV-Template-Matching/assets/36232582/a595f560-921b-477f-837f-ed0b1d75f631)
![config2](https://github.com/Clayton-Klemm/Pokemon-Yellow-Gambling-With-OpenCV-Template-Matching/assets/36232582/89f521e7-a9bc-4ff9-8f32-dda6013dd937)
![config3](https://github.com/Clayton-Klemm/Pokemon-Yellow-Gambling-With-OpenCV-Template-Matching/assets/36232582/aebfb789-0474-43dc-a3e7-aa5b1090e334)
![linker](https://github.com/Clayton-Klemm/Pokemon-Yellow-Gambling-With-OpenCV-Template-Matching/assets/36232582/da0de8cf-c42e-4221-b00d-c8bd711db75c)

Be sure to have your dll files in their approporiate folders. for example if you are in debug mode the opencv_worldXXXd.dll should be in your debug path directory of your project
likewsie for the release (the non *d.dll) should be in release
C:\yourPath\pokemonYellowGambling\x64\Debug\opencv_world481d.dll
C:\yourPath\pokemonYellowGambling\x64\Release\opencv_world481.dll

Next you will want to switch your compiler to C++v17+ because std::filesystem was not added until C++v17
You can change this by:
![iso standard](https://github.com/Clayton-Klemm/Pokemon-Yellow-Gambling-With-OpenCV-Template-Matching/assets/36232582/f0710c39-0eaf-413c-a79a-1f25548f231d)
