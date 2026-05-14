@echo off
echo ============================================
echo  EcoGest -- Gerando executavel para Windows
echo ============================================
echo.

gcc -Wall -std=c11 -static ^
    src/main.c src/auth.c src/cliente.c src/funcionario.c ^
    src/residuo.c src/relatorio.c src/utils.c ^
    -o EcoGest.exe

if %errorlevel% == 0 (
    echo.
    echo  Sucesso! Arquivo gerado: EcoGest.exe
    echo.
    echo  Para distribuir, basta enviar o EcoGest.exe
    echo  O usuario final nao precisa instalar nada.
    echo.
    echo  Execute agora? [S/N]
    set /p resp=
    if /i "%resp%"=="S" start EcoGest.exe
) else (
    echo.
    echo  ERRO na compilacao.
    echo  Verifique se o GCC esta instalado e no PATH.
    echo  Instale pelo MSYS2: pacman -S mingw-w64-x86_64-gcc
)
pause
