 # Changelog:

 ## Major Features & Systems:
   * The Block Registry (Data-Driven Architecture):
       * Implemented the Flyweight pattern (BlockProperties) to centralize block data. Millions of blocks in the world no longer store redundant data.
   * Triple-Layer Rendering System:
       * Decoupled visual textures from gameplay Block IDs.
       * Implemented a step-by-step rendering loop:
           1. Background Layer: Draws persistent background walls (Row 1 of the atlas).
           2. Foreground Layer: Draws the solid base block (Row 0 of the atlas).
           3. Ore Layer: Draws transparent ore overlays (Row 2 of the atlas).
       * Added an Ore namespace/enum to easily apply the same ore texture across different base blocks (e.g., Coal in Shallow vs. Coal in Deepslate).
   * Progressive Mining:
       * Mining is no longer instantaneous. Added a progressive mining system based on block durability and player pickaxePower.
       * Visual Feedback: Added a 4-frame block-cracking overlay animation that responds to mining progress.
       * Mining Range: Implemented a physical distance restriction (radius of 2.5 blocks) for mining and placing blocks.
   * Depth-Dependent Ore Generation:
       * Replaced flat percentage spawn rates with linear interpolation graphs (SpawnPoint).
       * Ores now smoothly fade in and out based on depth (Y-level) rather than hard layer cut-offs.

  ## Enhancements:
   * Cave backgrounds: The GenerationOracle now automatically assigns appropriate background wall textures to naturally generated caves, preventing empty sky voids underground.
   * Inventory Grouping: Added the dropType property. Mining block variants (like stone_shallow or stone_deepslate) now correctly stacks them as a single item type in the player's inventory.
   * Coordinate HUD: Added a X/Y coordinate tracker at the top of the screen.
   * Atlas Organization: Standardized the TextureSheet.png layout:
       * Row 0: Solid Foregrounds
       * Row 1: Backgrounds
       * Row 2: Transparent Ores
       * Row 3: Selection Box (and other miscellaneous things)
       * Row 4: Breaking Animations
