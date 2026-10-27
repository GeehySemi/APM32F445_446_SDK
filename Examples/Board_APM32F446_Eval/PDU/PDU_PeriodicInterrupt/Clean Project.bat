rd /Q /S Project\MDK\Out
rd /Q /S Project\MDK\Objects
rd /Q /S Project\MDK\Listings
rd /Q /S Project\MDK\DebugConfig
del /Q Project\MDK\*.bak
del /Q Project\MDK\*.dep
del /Q Project\MDK\JLink*
del /Q Project\MDK\*.uvguix.*
del/Q Project\MDK\*.uvoptx

del /Q Project\IAR\*.dep
del /Q Project\IAR\*.ewd
del /Q Project\IAR\*.ewt
del /Q Project\IAR\*.dep
del /Q Project\IAR\Project.dep
del /Q Project\IAR\Debug
del /Q Project\IAR\Flash
del /Q Project\IAR\settings
del /Q Project\IAR\Debug
rd  /Q /S Project\IAR\Flash
rd /Q /S Project\IAR\settings
rd /Q /S Project\IAR\Debug
rd /Q /S Project\IAR\APM32F446
rd /Q /S Project\IAR\APM32F446_DUGRAM

rd /Q /S Project\Eclipse\.settings
rd /Q /S Project\Eclipse\APM32F446
del /Q Project\Eclipse\*.gitignore