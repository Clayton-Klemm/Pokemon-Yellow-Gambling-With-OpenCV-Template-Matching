# Pokemon Yellow Gambling
# Update!!! THE GAME CHEATS! :( 
I thought it was a performance issue. However, after extensive testing, I came to the unfortunate truth that the game cheats. Upon looking at the decompiled assembly code of the ROM, my suspicion has been confirmed with the **'GameCornerSelectLuckySlotMachine'** script. [Pokémon Yellow ROM Decompile](https://github.com/pret/pokeyellow/blob/master/scripts/GameCorner.asm).

![Game Window](https://github.com/Clayton-Klemm/Pokemon-Yellow-Gambling-With-OpenCV-Template-Matching/assets/36232582/a6b19e65-681a-477b-af18-ebf491f0a85f)

https://github.com/Clayton-Klemm/Pokemon-Yellow-Gambling-With-OpenCV-Template-Matching/assets/36232582/7b831f33-0f21-4158-97ad-ebf61926d7c4



This project uses **OpenCV**. Ensure you have it installed and properly configured.

### Installation and Configuration:

1. **Install OpenCV**:  
   - Visit [OpenCV's official site](https://opencv.org/get-started/) to get started with the installation.

2. **Configure Visual Studio for OpenCV**:  
   Once you've installed OpenCV, you'll need to configure Visual Studio to link with its DLL files.

   ![Right Click Project Properties](https://github.com/Clayton-Klemm/Pokemon-Yellow-Gambling-With-OpenCV-Template-Matching/assets/36232582/9905db2a-a131-4cba-bbcc-87617447c7b1)
   ![Config 1](https://github.com/Clayton-Klemm/Pokemon-Yellow-Gambling-With-OpenCV-Template-Matching/assets/36232582/a595f560-921b-477f-837f-ed0b1d75f631)
   ![Config 2](https://github.com/Clayton-Klemm/Pokemon-Yellow-Gambling-With-OpenCV-Template-Matching/assets/36232582/89f521e7-a9bc-4ff9-8f32-dda6013dd937)
   ![Config 3](https://github.com/Clayton-Klemm/Pokemon-Yellow-Gambling-With-OpenCV-Template-Matching/assets/36232582/aebfb789-0474-43dc-a3e7-aa5b1090e334)
   ![Linker](https://github.com/Clayton-Klemm/Pokemon-Yellow-Gambling-With-OpenCV-Template-Matching/assets/36232582/da0de8cf-c42e-4221-b00d-c8bd711db75c)

3. **Organize DLL files**:  
   Ensure the DLL files are placed in the appropriate folders. For example:

   - Debug mode: `opencv_worldXXXd.dll` should be in your project's debug path directory.
   - Release mode: `opencv_worldXXX.dll` should be in your project's release path directory.

     ```plaintext
     C:\yourPath\pokemonYellowGambling\x64\Debug\opencv_world481d.dll
     C:\yourPath\pokemonYellowGambling\x64\Release\opencv_world481.dll
     ```

4. **Switch Compiler to C++v17+**:  
   This project requires C++v17 or higher due to its reliance on `std::filesystem`. To switch:

   ![ISO Standard](https://github.com/Clayton-Klemm/Pokemon-Yellow-Gambling-With-OpenCV-Template-Matching/assets/36232582/f0710c39-0eaf-413c-a79a-1f25548f231d)
