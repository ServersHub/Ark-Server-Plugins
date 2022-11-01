# SimpleBan
 Source for my Ark: Survival Evolved mod called Simple Ban. The mod allows you to keep Steam Family Sharing enabled while preventing that being used to bypass your ban list/s.

## License
 Creative Commons Zero v1.0 Universal 
 _(do as you like with this, I am not responsible for what you do)_

## Workshop Page
 Mod description explains bit further how it works and discussions cover INI settings / scriptcommands.
 https://steamcommunity.com/sharedfiles/filedetails/?id=2202550947

## Usage in ADK
 To use this source in ADK follow these basic steps:
 - Close the editor
 - Copy the 'SimpleBan' folder into your kit's \Mods folder
 - Open the editor

## Functionality Contained
- Persistent Singleton CCA
  * Pretty much the entire mod is the CCA
  * Highlander Check
  * Auto-Respawn on Destroyed
- HTTP Get & Response for multiple purposes
  * Optional Json Config Url 
  * Ban List Url/s
  * Steam Web API
- Custom Scriptcommands
  * In-Game Console or RCON
- ServerGameLog
  * Live Logging to ShooterGame/Saved/Logs/ShooterGame.log
  * DebugMode setting (verbose dev logging)
  * Settings/Banlist init, Scriptcommands, Banned Kicks, etc
- SaveGame (*.SAV files)
  * Ban Log, which can be viewed via scriptcommand
  * Player Ark Ownership, for efficiency to call Steam Web API only as needed (since it rate limits per api key)
- Hash Functions
  * Example usage of hash lib I previously shared