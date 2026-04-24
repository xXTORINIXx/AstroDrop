# 🎮 AstroDrop

# MultiplayerSessions - Lobby completo para Unreal Engine 5.7

## O que entrou nesta versão
- Login EOS com fallback: `PersistentAuth -> AccountPortal`
- Helper para dedicated server: `LoginDedicatedServer()` e `AutoLoginForCurrentRuntime()`
- Sessões com lobby e voz automática quando disponível
- Chat de lobby replicado pelo `GameState`
- Ready state por jogador
- Voice toggle local + replicação do estado no `PlayerState`
- Menu UMG preparado para exibir lobby, chat, ready e voz

## Classes novas
- `AMultiplayerLobbyGameMode`
- `AMultiplayerLobbyGameState`
- `AMultiplayerLobbyPlayerController`
- `AMultiplayerLobbyPlayerState`
- `FMultiplayerLobbyPlayerInfo`
- `FMultiplayerChatMessage`

## Para usar no mapa de Lobby
No mapa de lobby, configure o GameMode para:
- GameMode: `AMultiplayerLobbyGameMode`
- PlayerController: já vem pelo GameMode
- GameState: já vem pelo GameMode
- PlayerState: já vem pelo GameMode

## Widgets opcionais do Menu
O `UMenu` agora procura, opcionalmente, por estes nomes de widgets no `WBP_MenuWidget`:
- `HostButton`
- `JoinButton`
- `ReadyButton`
- `LeaveButton`
- `ToggleVoiceButton`
- `SendChatButton`
- `RefreshLobbyButton`
- `StatusText`
- `PlayerNameText`
- `LobbyStateText`
- `ReadyStateText`
- `VoiceStateText`
- `PlayerListText`
- `ChatHistoryText`
- `ChatInputTextBox`

Se o seu widget atual não tiver todos, o plugin continua funcionando, só sem mostrar aquela parte.

## Configuração recomendada no DefaultEngine.ini
```ini
[OnlineSubsystem]
DefaultPlatformService=EOS

[OnlineSubsystemEOS]
bEnabled=true
bUseEAS=true
bUseEOSConnect=true
bUseEOSSessions=true
bUseEOSVoiceChat=true
```

## Dedicated Server
Esta versão adiciona suporte base para dedicated server usando login `developer` com Dev Auth Tool.

Exemplo de linha de comando:
```bash
MyGameServer.exe -server -log -EOSDevAuthHost=127.0.0.1:8081 -EOSDevAuthUser=DedicatedServer
```

## Observações importantes
1. Para voz funcionar de verdade, o projeto precisa estar com EOS Voice Chat habilitado e corretamente configurado pela Epic. O plugin já seta `bUseLobbiesVoiceChatIfAvailable` na sessão.
2. O chat de texto foi implementado com replicação do `GameState`, então ele funciona tanto em listen server quanto em dedicated server.
3. O plugin foi preparado para UE 5.7, mas não foi compilado neste ambiente. Depois de substituir, abra o projeto e deixe a Unreal regenerar os arquivos e compilar o módulo.

