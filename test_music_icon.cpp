#include "src/IconProvider.hpp"
#include "src/FileOperations.hpp"
#include <iostream>
#include <cassert>

int main() {
    IconProvider provider;
    
    // Create a mock FileInfo for a music directory
    fs::path music_path("Music");
    FileInfo music_dir(music_path);
    music_dir.is_directory = true;
    
    // Get the icon for the music directory
    std::string icon = provider.getIcon(music_dir);
    
    // Check that we get the musical note icon
    assert(icon == "\ufb2c");
    
    // Create a mock FileInfo for an audio directory
    fs::path audio_path("Audio");
    FileInfo audio_dir(audio_path);
    audio_dir.is_directory = true;
    
    // Get the icon for the audio directory
    std::string audio_icon = provider.getIcon(audio_dir);
    
    // Check that we get the musical note icon
    assert(audio_icon == "\ufb2c");
    
    std::cout << "All tests passed! Music folder icons are working correctly." << std::endl;
    std::cout << "Music folder icon: " << icon << std::endl;
    std::cout << "Audio folder icon: " << audio_icon << std::endl;
    
    return 0;
}