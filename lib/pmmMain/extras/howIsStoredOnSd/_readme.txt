*thisIsAVariable*

Minerva Rockets: The first directory. It's MY team, ok?
 |
 |-> pmm
 |    |
 |    |-> systemName_*sessionId*
 |    |    |
 |    |    |-> _self
 |    |    |
 |    |    |-> *sourceAddress*
 |    |    |    |
 |    |    |    |-> Session_*sessionId*

Minerva Rockets:
The first directory. My team name ;)

pmm:
This System software. https://en.wikipedia.org/wiki/System_software

systemName_*sessionId*:
systemName is the name you gave to it in the pmmConsts.h.
sessionId is at which system's session the data were generated/received. Is represented in decimal format, 3 characters.
Ex: AuroraAvionic_03

_self:
Inside it will be stored the data produced by the system itself.

*sourceAddress*:
Inside it will be stored the data received by telemetry. Is represented in decimal format, 3 characters.
Ex: 014

Session_*sessionId*
As the emissor of the data may change his current session while your receiver is on, and we don't want to mix them.
Ex: Session_03


