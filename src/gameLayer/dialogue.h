#ifndef PIXEL_CAVERNS_DIALOGUE_H
#define PIXEL_CAVERNS_DIALOGUE_H

#pragma once
#include <vector>
#include <string>

enum DialogueState {
    DIALOGUE_IDLE,
    DIALOGUE_CHOOSING,
    DIALOGUE_TALKING
};

inline DialogueState diagState = DIALOGUE_IDLE;

inline bool hasEnteredDeepslate = false;
inline bool isDialogueActive = false;
inline std::vector<std::string> activeDialogue;
inline int dialoguePageIndex = 0;
inline int dialogueCharCount = 0;
inline float dialogueTimer = 0.0f;
inline float textSpeed = 0.03f;
inline bool hasSeenIntro = false;

const std::vector<std::string> introDialogue = {
    "*This dialogue is reserved for the school presentation*",
};

const std::vector<std::string> choosingDialogue = {
    "What would you like to know about?"
};

const std::vector<std::string> overviewDialogue = {
    "Welcome to Pixel Caverns!",
    "I will be serving as your guide here, you can always come here\nto check for information.",
    "Before anything, this game is currently in a very early\nrelease version, so there's only two layers to explore at the moment.",
    "The first things you'll notice after talking with me are the\nelements of the base. You can dump your ores in the storage when you're\nstarting to apprach your capacity limit.\nYou have almost limitless capacity here.",
    "Next, the forge for crafting and equipping new pickaxes.\nFor now, this is your only and primary objective until more content is added.",
    "You can also take a nap, which will make the mine regenerate.\nSpawning right over the tunnels you've made can be annoying!",
    "Lastly, there's the exit door for leaving to the outside world.\nDon't worry, you'll always have a working teleport button\nto come right back here.",
    "Now go out there and explore!\nRemember to check back with me for tips."
};

const std::vector<std::string> oresDialogue = {
    "As of now, there's currently 5 ores in total. I'll tell you about the ones\ncurrently relevant to you.",
    "Coal is, well, coal, it's very easy to find it. I recommend that you search\naround the middle of your current layer.",
    "Iron is also fairly common, though you'll have to dig bit deeper for it.",
    "Silver could be considered a lucky find. Even though it does spawn in your\ncurrent layer, it's a tad rare. You'll find it more often later.",
    "That's all you should focus for now, come back to me once you've\ndiscovered a new layer."
};

const std::vector<std::string> oresDeepslateDialogue = {
    "Interesting, you've reached the Deepslate layer sooner than I anticipated.\nIt's only appropriate for me to now tell you about the rest of the ores.",
    "Currently, gold and diamond are exclusive to Deepslate. My research shows\nthat gold is likely to behave like iron when it comes it's most common depth.\nI mean, searching around the middle part of the layer, of course.",
    "Diamonds will be a rare find, the most I can tell you from my observations\nis to search as deep as possible. It should increase your odds, even\nif it's not by much.",
    "Besides those two, the ores you already know seem to have different\nspawn rates. Coal and iron are less frequent, but silver gets a boost."
};

const std::vector<std::string> tipsDialogue = {
    "The best tip I can give you right now is to observe the spawn rates\nof ores. If you haven't noticed, their density heavily depends on depth.",
    "Remember that you can also check with me for clues related\nto figuring out the spawn rates.",
    "Other than that, I'll be waiting for you when you reach the next layer."
};

const std::vector<std::string> tipsDeepslateDialogue = {
    "Hope you didn't get scared by the change in ambience.\nThose caves have a scary echo!",
    "You've also definitely noticed the rapid decrease in light.\nThankfully, you have a little lantern by your side. Even though it's not\nthe strongest, it's still better than nothing.",
    "However, this isn't necessarily a bad thing, because the\nnew ores present here also glow! This means it'll be actually\neasier to notice them.",
    "As an actual tip, stick between the Shallow and the Deepslate\nlayers if you're looking for the other non-glowing ores.\nIt would be horrid to search for them in almost complete darkness.",
    "... unless you know there's a way better spawn rate."
};

#endif
