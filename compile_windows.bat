@echo off
gcc -Wall -std=c11 src/main.c src/auth.c src/cliente.c src/funcionario.c src/residuo.c src/relatorio.c src/utils.c -o ecogest.exe
if %errorlevel% == 0 (
    echo Compilacao bem-sucedida! Execute ecogest.exe
) else (
    echo ERRO na compilacao. Verifique o GCC instalado.
)
pause
