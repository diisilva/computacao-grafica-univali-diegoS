# 🧠 Setup ML Environment — Drive X: com PyTorch + CUDA

> Documentação gerada a partir da sessão de setup em 08/03/2026.  
> GPU: NVIDIA GeForce RTX 3060 | CUDA 13.0 | Windows 11 | Python 3.12

## 🐧 Atualização Linux (Ubuntu 24.04) — RTX 4050

> Atualizado em 15/03/2026 com base no ambiente: driver `nvidia-driver-580-open`.

### Compatibilidade CUDA/PyTorch

- A RTX 4050 funciona normalmente com os wheels `cu126` do PyTorch.
- Com driver NVIDIA série 580, manter `cu126` é uma escolha segura.

### Fluxo recomendado no Linux (corrige `python`/`pip` no venv)

```bash
# 0) Dependências base do Python no Ubuntu
sudo apt update
sudo apt install -y python3 python3-venv python3-pip

# opcional: habilita o comando "python" como alias de python3
sudo apt install -y python-is-python3

# 1) recriar venv limpa (no seu caso, em ~/projects_ubuntu)
cd ~/projects_ubuntu
rm -rf .venv
python3 -m venv .venv
source .venv/bin/activate

# 2) garantir pip dentro da venv
python3 -m ensurepip --upgrade
python3 -m pip install --upgrade pip setuptools wheel

# 3) instalar PyTorch com CUDA 12.6
python3 -m pip install torch torchvision torchaudio --index-url https://download.pytorch.org/whl/cu126

# 4) Jupyter + kernel apontando para esta venv
python3 -m pip install jupyterlab ipykernel
python3 -m ipykernel install --user --name=ml-env --display-name "Python (venv)"

# 5) conferir se o kernel foi registrado
python3 -m jupyter kernelspec list
```

### Validar GPU e CUDA

```bash
nvidia-smi
python3 -c "import torch; print('PyTorch:', torch.__version__); print('torch CUDA:', torch.version.cuda); print('CUDA disponível:', torch.cuda.is_available()); print('GPU:', torch.cuda.get_device_name(0) if torch.cuda.is_available() else 'Nenhuma')"
```

Resultado esperado (exemplo):

```text
PyTorch: 2.x.x+cu126
torch CUDA: 12.6
CUDA disponível: True
GPU: NVIDIA GeForce RTX 4050 Laptop GPU
```

### Migrar pacotes de uma venv antiga para a nova

Se a venv antiga ainda existir, exporte os pacotes e reinstale tudo na venv nova:

```bash
# exemplo de venv antiga
OLD_VENV="/home/diego/computacao_grafica/.venv"

# 1) exportar pacotes da antiga
"$OLD_VENV/bin/python" -m pip freeze > ~/projects_ubuntu/requirements_old_venv.txt

# 2) ativar a venv nova
cd ~/projects_ubuntu
source .venv/bin/activate

# 3) reinstalar pacotes antigos
python3 -m pip install -r requirements_old_venv.txt

# 4) instalar também pacotes novos do ambiente ML
python3 -m pip install torch torchvision torchaudio --index-url https://download.pytorch.org/whl/cu126
python3 -m pip install transformers sentence-transformers scikit-learn pandas numpy imbalanced-learn accelerate ipywidgets jupyterlab ipykernel
```

Se o caminho da venv antiga não existir, use seu histórico/snapshot de `requirements.txt` para repor os pacotes manualmente.

---

## 📁 1. Criar estrutura de pastas no drive X:

```powershell
New-Item -ItemType Directory -Force -Path "X:\ml-env"
New-Item -ItemType Directory -Force -Path "X:\projects"
cd X:\ml-env
```

---

## 🐍 2. Criar e ativar a venv

```powershell
python -m venv .venv
```

### Habilitar execução de scripts (se necessário):

```powershell
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
```

### Ativar a venv:

```powershell
X:\ml-env\.venv\Scripts\Activate.ps1
```

O prompt muda para `(.venv)` confirmando ativação.

---

## ⬆️ 3. Atualizar pip

```powershell
python -m pip install --upgrade pip setuptools wheel
```

---

## 🔥 4. Instalar PyTorch com CUDA 12.6

> Usar `cu126` — compatível com driver CUDA 13.0 da RTX 3060.

```powershell
pip install torch torchaudio torchvision --index-url https://download.pytorch.org/whl/cu126
```

### Verificar reconhecimento da GPU:

```powershell
python -c "import torch; print('PyTorch:', torch.__version__); print('CUDA:', torch.cuda.is_available()); print('GPU:', torch.cuda.get_device_name(0) if torch.cuda.is_available() else 'ERRO')"
```

**Resultado esperado:**
```
PyTorch: 2.6.0+cu126
CUDA: True
GPU: NVIDIA GeForce RTX 3060
```

---

## 📦 5. Instalar pacotes do projeto ML

```powershell
pip install transformers sentence-transformers scikit-learn pandas numpy
pip install imbalanced-learn
pip install accelerate
pip install ipywidgets
```

---

## 📓 6. Instalar e configurar JupyterLab

```powershell
pip install jupyterlab
pip install ipykernel
python -m ipykernel install --user --name=ml-env --display-name "Python (ml-env)"
```

### Ativar tema dark no JupyterLab:
`Settings` → `Theme` → **JupyterLab Dark**

---

## 🚀 7. Iniciar JupyterLab em um projeto

```powershell
X:\ml-env\.venv\Scripts\Activate.ps1
cd X:\projects\nome-do-projeto
jupyter lab
```

### Atalho: criar `start.ps1` na pasta do projeto:

```powershell
# X:\projects\nome-do-projeto\start.ps1
X:\ml-env\.venv\Scripts\Activate.ps1
jupyter lab
```

Rodar com:
```powershell
.\start.ps1
```

---

## ✅ 8. Testar GPU no notebook

Selecione o kernel **"Python (ml-env)"** no JupyterLab e execute:

```python
import torch

print("PyTorch:", torch.__version__)
print("CUDA disponível:", torch.cuda.is_available())
print("GPU:", torch.cuda.get_device_name(0) if torch.cuda.is_available() else "Nenhuma GPU detectada")
print("VRAM total:", round(torch.cuda.get_device_properties(0).total_memory / 1024**3, 1), "GB")
```

**Resultado esperado:**
```
PyTorch: 2.10.0+cu126
CUDA disponível: True
GPU: NVIDIA GeForce RTX 3060
VRAM total: 12.0 GB
```

---

## 🗂️ 9. Estrutura final do drive X:

```
X:\
├── ml-env\
│   └── .venv\          ← venv compartilhada (torch + CUDA aqui)
└── projects\
    ├── salary-survey-ml\
    ├── StyleTTS\
    └── outros-projetos\
```

---

## ⚙️ 10. Configurações importantes para Windows

### `os.environ` — colocar ANTES do `import torch`:

```python
import os
os.environ["PYTORCH_CUDA_ALLOC_CONF"] = "max_split_size_mb:512"
# Nota: "expandable_segments:True" NÃO funciona no Windows
```

### `TrainingArguments` otimizado para Windows + RTX 3060:

```python
from transformers import TrainingArguments

training_args = TrainingArguments(
    output_dir='./results',
    num_train_epochs=3,
    per_device_train_batch_size=14,
    gradient_accumulation_steps=2,    # equivale a batch_size=28 sem custo de VRAM
    per_device_eval_batch_size=16,
    learning_rate=2e-5,
    weight_decay=0.01,
    eval_strategy="epoch",
    save_strategy="epoch",
    logging_steps=50,
    load_best_model_at_end=True,
    metric_for_best_model="f1",
    greater_is_better=True,
    warmup_ratio=0.1,
    fp16=True,                         # ← habilita FP16, reduz VRAM e acelera treino
    lr_scheduler_type="cosine",
    dataloader_num_workers=0,          # ← ESSENCIAL no Windows (evita erro de multiprocessing)
    report_to="none",                  # ← desativa wandb/tensorboard
)
```

### Por que `dataloader_num_workers=0`?
No Linux, o Python usa `fork` para paralelismo. No Windows usa `spawn`, que não funciona dentro do Jupyter — `num_workers=0` desativa o multiprocessing e resolve o problema.

---

## 🔍 11. Monitorar recursos durante treino

### RAM (PowerShell):

```powershell
while ($true) {
    $mem = Get-CimInstance Win32_OperatingSystem
    $used = [math]::Round(($mem.TotalVisibleMemorySize - $mem.FreePhysicalMemory)/1MB, 1)
    $total = [math]::Round($mem.TotalVisibleMemorySize/1MB, 1)
    Write-Host "RAM: $used GB / $total GB"
    Start-Sleep 2
}
```

### VRAM da GPU (PowerShell):

```powershell
while ($true) {
    nvidia-smi --query-gpu=memory.used,memory.free --format=csv,noheader
    Start-Sleep 2
}
```

---

## 🧹 12. Liberar VRAM após treino (opcional)

```python
import gc

del model
del trainer
del train_dataset
del val_dataset

gc.collect()
torch.cuda.empty_cache()

print(f"VRAM livre: {torch.cuda.memory_reserved(0)/1024**3:.1f} GB")
```

---

## ⚠️ Warnings conhecidos (podem ser ignorados)

| Warning | Causa | Ação |
|---|---|---|
| `warmup_ratio is deprecated` | transformers v5.2 ainda não lançado | Ignorar por ora |
| `logging_dir is deprecated` | idem | Ignorar por ora |
| `expandable_segments not supported` | Não funciona no Windows | Usar `max_split_size_mb:512` |
| `UNEXPECTED keys` ao carregar BERT | Cabeça MLM descartada (normal) | Ignorar |
| `MISSING: classifier.weight/bias` | Camada de classificação nova, será treinada | Ignorar |
| `IProgress not found` | ipywidgets não instalado | `pip install ipywidgets` |

---

## 🐞 Debug CUDA (usar apenas para rastrear erros)

```python
import os
os.environ['CUDA_LAUNCH_BLOCKING'] = '1'
# ⚠️ Deixar comentado em produção — torna o treino 3-5x mais lento
```

---

## 🧯 Troubleshooting rápido (Linux)

### Erro: `python: comando não encontrado`

- Use `python3` nos comandos; no Ubuntu isso é o padrão.
- Opcional para manter compatibilidade com guias antigos:

```bash
sudo apt install -y python-is-python3
```

### Erro: `/usr/bin/python3: No module named pip`

- O `pip` da sua venv não foi criado corretamente.
- Ative a venv e rode:

```bash
python3 -m ensurepip --upgrade
python3 -m pip install --upgrade pip setuptools wheel
```

- Se ainda falhar, recrie a venv após instalar `python3-venv`:

```bash
sudo apt install -y python3-venv
rm -rf .venv
python3 -m venv .venv
source .venv/bin/activate
python3 -m ensurepip --upgrade
```

---

*Documentação gerada em 08/03/2026 — Sessão de setup ML Environment Windows 11 + RTX 3060*
