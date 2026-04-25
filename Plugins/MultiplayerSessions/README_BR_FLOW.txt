MultiplayerSessions - Battle Royale Flow
=======================================

Versão ajustada para Unreal Engine 5.7.x / EOS.

Fluxo principal:
1. MenuSetup() autentica no EOS.
2. Ao logar, o subsystem cria uma Party solo automaticamente.
3. ReadyButton alterna o ready da Party/local player.
4. PlayButton, ou HostButton no WBP antigo, chama StartBattleRoyaleMatchmaking().
5. O subsystem busca uma sessão BattleRoyale em fase Staging.
6. Se encontrar, entra nela.
7. Se não encontrar, cria uma nova sessão BattleRoyale.
8. Ao criar, o host faz ServerTravel para o LobbyPath informado no MenuSetup.
9. O seu LobbyGameMode decide quando todos estão ready e faz ServerTravel para o mapa da partida.

Compatibilidade com seu WBP atual:
- HostButton continua funcionando, mas agora age como PLAY.
- JoinButton continua funcionando como busca/debug de lobby existente.
- Se criar novo WBP, pode adicionar PlayButton e PartyListText.

Configuração recomendada:
MenuSetup(
    100,
    "BattleRoyale",
    "/Game/Maps/L_AstroDropLobby"
)

No mapa de lobby/staging:
- GameMode Override: BP_AstroDropLobbyGameMode
- Use Seamless Travel: ON
- PlayerState: Lobby PlayerState
- PlayerController: Lobby PlayerController

No mapa da partida:
- GameMode Override: BP_AstroDropGameMode
- PlayerState e GameState próprios da partida, não os do lobby.
