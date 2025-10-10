# AnyText

Change any text in the game through a list of find and replace instructions.

<img src="cover.png" width="500" />


## Known Bugs

- Replacing "practice" will decapitalize many buttons
- Replacing a menu banner will decapitalize all menu banners
- Creating many rows will make the buttons lag


## v1.1.0 To-Do

- [x] Create `FindReplaceEntry` class
- [x] Create `Config` class
- [x] Load config files
- [x] Create `TextManager` component to handle text state
- [x] Create `TextIdentifier` to add and remove `TextManager` component
- [x] Load config files in specific order
- [x] Set up `AnyTextFlowCoordinator`
- [x] Set up `ConfigListViewController`
- [x] Display each config as a row
- [x] Set up `EntryListViewController`
- [x] Display each entry as a row
  - [x] Add input field for find string
  - [x] Add input field for replace string
  - [x] Add button to open find settings
  - [x] Add button to open replace settings
  - [x] Add buttons to reorder entry
  - [x] Add button to remove entry
- [x] Add modal for find settings
- [ ] Add dropdown for find algorithm
- [x] Add toggle for accumulation
- [ ] Add modal for replace settings
- [ ] Add toggles to configs
- [ ] Add button to create new config
- [ ] Add button to create new entry
- [ ] Add find with match case
- [ ] Add replace all or partial
- [ ] Add replace text style settings
- [ ] Add position offset
- [ ] Add proper icons to buttons
- [ ] ~~Fix large text being hidden behind ellipsis~~ (Tried a bunch of text options, TMPro doesn't seem to account for text overflow when right aligned)
- [ ] Stop text identification on AnyText-related pieces of text
- [ ] Trigger text updates upon exiting AnyText menu
- [ ] Add hover hints
- [x] Upgrade v1.0.1 configs
- [x] Investigate `private System.Collections.IEnumerator global::UIKeyboardManager.SummonSystemKeyboardAndWaitForResponse(HMUI.InputFieldView input)`
  - [ ] Note that the game needs to be repatched with `oculus.software.overlay_keyboard` but IT DOES WORK HECK YEAAAHHH
- [ ] Load fonts


## Build Instructions

Use `qpm s build` to build
Same goes for `qpm s copy` and `qpm s qmod`


## Credits

* [zoller27osu](https://github.com/zoller27osu), [Sc2ad](https://github.com/Sc2ad) and [jakibaki](https://github.com/jakibaki) - [beatsaber-hook](https://github.com/sc2ad/beatsaber-hook)
* [raftario](https://github.com/raftario)
* [Lauriethefish](https://github.com/Lauriethefish), [danrouse](https://github.com/danrouse) and [Bobby Shmurner](https://github.com/BobbyShmurner) for [this template](https://github.com/Lauriethefish/quest-mod-template)
