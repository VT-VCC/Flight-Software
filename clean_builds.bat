@echo off

forfiles /m "build-*" /c "cmd /c if @isdir==TRUE rmdir /s /q @file"
