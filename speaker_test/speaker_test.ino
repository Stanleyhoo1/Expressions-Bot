#include "heheheha.h"
#include "mimimimi.h"
#include "hmm.h"
#include "uwu.h"
#include "wilheim_scream.h"
#include "yawn.h"
#include "snore.h"
#include "bruh.h"
#include "annoyed_hmm.h"

// 1. Array of pointers (Unchanged)
const unsigned char* const sound_table[] PROGMEM = {
  he_he_he_ha_clash_royale_deep_fried,
  mimimi_clash_royale,
  minecraft_villager_sound_effect,
  cute_uwu,
  VOXScrm_Wilhelm_scream_ID_0477__BigSoundBank_com,
  clash_royale_yawning_emote,
  man_snoring_meme_ctrllNn_1_,
  movie_1,
  new2009pro_minecraft_villager_sound_15065
};

// 2. Matching array for lengths
// FIX: Use sizeof() directly on the original array names here!
const unsigned int sound_lengths[] PROGMEM = {
  sizeof(he_he_he_ha_clash_royale_deep_fried),
  sizeof(mimimi_clash_royale),
  sizeof(minecraft_villager_sound_effect),
  sizeof(cute_uwu),
  sizeof(VOXScrm_Wilhelm_scream_ID_0477__BigSoundBank_com),
  sizeof(clash_royale_yawning_emote),
  sizeof(man_snoring_meme_ctrllNn_1_),
  sizeof(movie_1),
  sizeof(new2009pro_minecraft_villager_sound_15065)
};

const int TOTAL_SOUNDS = sizeof(sound_table) / sizeof(sound_table[0]);
void playSound(int soundIndex) {
  // Safety check: Make sure the index actually exists
  if (soundIndex < 0 || soundIndex >= TOTAL_SOUNDS) {
    Serial.println("Error: Sound index out of range!");
    return;
  }

  // Fetch pointer and length from PROGMEM
  const unsigned char* current_sound = (const unsigned char*) pgm_read_ptr(&sound_table[soundIndex]);
  unsigned int current_length = pgm_read_word(&sound_lengths[soundIndex]);

  // Print the results
  Serial.print("Ready to play sound #");
  Serial.print(soundIndex);
  Serial.print(" | Size: ");
  Serial.print(current_length);
  Serial.println(" bytes");

  // FIX: Don't use sizeof() here! We already fetched the exact length from PROGMEM above.
  unsigned int totalSamples = current_length; 
  
  for (long i = 0; i < totalSamples; i++) {
    // FIX: Use pgm_read_byte to safely extract the audio data from Flash memory
    byte audioSample = pgm_read_byte(&current_sound[i]);
    
    analogWrite(A0, audioSample); // Send the audio wave to pin A0
    delayMicroseconds(125);       // Wait exactly the right amount of time for 8000Hz
  }
}

void setup() {
  Serial.begin(9600); // FIX: Added this so your Serial.print statements actually work!
  
  // Tell the R4's DAC to use 8-bit mode, matching your 8-bit audio file
  analogWriteResolution(8); 
}

void loop() {
  int index = 0;
  while (1) {
    playSound(index % TOTAL_SOUNDS);
    index++;
    delay(3000); // Wait 3 seconds before playing the next song clip
  }
}