$thisIsAVariable

 Minerva-Rockets-PMM
 |
 | -> $systemName
      |
      | -> Session $sessionId
           |
           | -> _Self
           |
           | -> $sourceAddress
                |
                | -> Session $sessionId


Detailed (this got pretty ugly):


 Minerva Rockets - PMM:
 | ==================================================================================================================
 | The first directory. My team name and this platform name ;)                                                      |
 | ==================================================================================================================
 |
 |
 | -> $systemName:
      | =============================================================================================================
      | It is the name you gave to this system in the pmmConsts.h.                                                  |
      | Having it separated from the Session_$sessionId makes finding other systems easily.                         |
      | =============================================================================================================
      |
      |
      | -> Session_$sessionId:
           | ========================================================================================================
           | sessionId is at which system's session the data were generated/received.                               |
           | The $sessionId ss represented in decimal format. Will have at least 2 digits.                          |
           |                                                                                                        |
           | Ex: Session 03                                                                                         |
           | ========================================================================================================
           |
           |
           | -> _Self:
           |    | ===================================================================================================
           |    | Inside it will be stored the data produced by the system itself.                                  |
           |    | The _ before makes finding this directory easier, as in crescent ordering this will appear first. |
           |    | Having it under Session $sessionId make us able to know at which (this system) session the        |
           |    | telemetry data was received.                                                                      |
           |    | ===================================================================================================
           |
           |
           | -> $sourceAddress:
                | ===================================================================================================
                | Inside this will be stored the data received by telemetry.                                        |
                | Is represented in decimal format, 3 characters.                                                   |
                |                                                                                                   |
                | Ex: 014                                                                                           |
                | ===================================================================================================
                |
                |
                | -> Session $sessionId
                    | ==============================================================================================
                    | As the emissor of the data may change his current session while your receiver is on, and we  |
                    | don't want to mix them.                                                                      |
                    | Ex: Session_03                                                                               |
                    | ==============================================================================================


