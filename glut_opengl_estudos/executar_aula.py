"""Runner simples para executar as aulas GLUT/OpenGL por número.

Uso:
    python3 executar_aula.py --list
    python3 executar_aula.py --aula 3
"""

from __future__ import annotations

import argparse
import subprocess
import sys
from pathlib import Path

AULAS = {
    1: "01_janela_e_triangulo.py",
    2: "02_primitivas_e_sistema_de_coordenadas.py",
    3: "03_transformacoes_animacao_timer.py",
    4: "04_entrada_teclado_mouse.py",
    5: "05_reshape_viewport_projecao.py",
    6: "06_double_buffer_fps.py",
    7: "07_mini_projeto_sistema_solar_2d.py",
    8: "08_scene_graph_hierarquico_2d.py",
    9: "09_picking_colisoes_aabb.py",
    10: "10_sistema_particulas_blending.py",
    11: "11_jogo_pong.py",
    12: "12_jogo_snake.py",
    13: "13_jogo_breakout.py",
    14: "14_jogo_tetris.py",
    15: "15_jogo_galaga.py",
}


def list_lessons() -> None:
    """Exibe mapa de aulas disponíveis."""
    print("Aulas disponíveis:")
    for number, filename in AULAS.items():
        print(f"  {number}: {filename}")


def run_lesson(lesson_number: int) -> int:
    """Executa uma aula específica e retorna o código de saída."""
    base_dir = Path(__file__).resolve().parent
    script = base_dir / AULAS[lesson_number]

    if not script.exists():
        print(f"Arquivo não encontrado: {script}")
        return 1

    print(f"Executando aula {lesson_number}: {script.name}")
    completed = subprocess.run([sys.executable, str(script)], check=False)
    return completed.returncode


def main() -> int:
    """Entry point do runner."""
    parser = argparse.ArgumentParser(description="Runner de aulas GLUT/OpenGL")
    parser.add_argument("--list", action="store_true", help="Lista aulas disponíveis")
    parser.add_argument("--aula", type=int, choices=sorted(AULAS.keys()), help="Número da aula")
    args = parser.parse_args()

    if args.list:
        list_lessons()
        return 0

    if args.aula is None:
        parser.error("informe --aula N ou use --list")

    return run_lesson(args.aula)


if __name__ == "__main__":
    raise SystemExit(main())
