
ASTRODROP - MultiplayerSessions plugin BR Dedicated/EOS

SUBSTITUIÇÃO
1. Feche Unreal/Visual Studio.
2. Substitua a pasta Plugins/MultiplayerSessions pela pasta deste ZIP.
3. Delete Binaries/Intermediate do projeto e do plugin se necessário.
4. Regenerate Visual Studio project files.
5. Compile Development Editor.

WBP MENU
- HostButton agora é PLAY por compatibilidade.
- PlayButton é opcional se quiser criar botão novo.
- InviteButton abre o overlay real de convite EOS para a sessão atual.
- JoinButton ficou como DEBUG JOIN / busca sessão.
- ReadyButton marca Pronto.

IMPORTANTE SOBRE INVITE EOS
O invite real do EOS abre o overlay da sessão atual. Então o fluxo recomendado para testar em 2 máquinas é:
1. Suba o Dedicated Server.
2. Máquina A: abre o jogo, clica PLAY e entra no staging lobby.
3. Máquina A: clica INVITE FRIEND para abrir o overlay EOS e convidar a conta da Máquina B.
4. Máquina B aceita o invite pelo overlay/Epic.
5. Ambos clicam READY.
6. Quando todos estiverem ready e atingir PlayersToStartMatch, o LobbyGameMode viaja para MatchMapPath.

DEDICATED SERVER
Crie no projeto Source/AstroDropServer.Target.cs:

using UnrealBuildTool;
using System.Collections.Generic;

public class AstroDropServerTarget : TargetRules
{
    public AstroDropServerTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Server;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        ExtraModuleNames.Add("AstroDrop");
    }
}

Build: Development Server
Run exemplo:
AstroDropServer.exe L_AstroDropLobby -log

MAPAS
No BP do MultiplayerLobbyGameMode ou classe filha:
- PlayersToStartMatch: 2 para teste
- MatchMapPath: /Game/Maps/L_BattleRoyale ou seu mapa real
- bAutoStartWhenAllReady: true

MENU SETUP
MenuSetup(100, "BattleRoyale", "/Game/Maps/L_AstroDropLobby");

FLUXO BR
Login EOS -> Party solo local -> PLAY procura dedicated session -> entra no Staging Lobby -> Invite EOS opcional -> Ready -> Match.
