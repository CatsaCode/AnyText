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
- [ ] Display each entry as a row
  - [ ] Add input field for find string
  - [ ] Add input field for replace string
  - [ ] Add button to open find settings
  - [ ] Add button to open replace settings
  - [ ] Add buttons to reorder entry
- [ ] Add modal for find settings
- [ ] Add dropdown for find algorithm
- [ ] Add toggle for accumulation
- [ ] Add modal for replace settings
- [ ] Add toggles to configs
- [ ] Add button to create new config
- [ ] Add button to create new entry
- [ ] Add find with ignore case
- [ ] Add replace all or partial
- [ ] Add replace text style settings
- [ ] Load fonts


## Build Instructions

Use `qpm s build` to build
Same goes for `qpm s copy` and `qpm s qmod`


## Credits

* [zoller27osu](https://github.com/zoller27osu), [Sc2ad](https://github.com/Sc2ad) and [jakibaki](https://github.com/jakibaki) - [beatsaber-hook](https://github.com/sc2ad/beatsaber-hook)
* [raftario](https://github.com/raftario)
* [Lauriethefish](https://github.com/Lauriethefish), [danrouse](https://github.com/danrouse) and [Bobby Shmurner](https://github.com/BobbyShmurner) for [this template](https://github.com/Lauriethefish/quest-mod-template)
