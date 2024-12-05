import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
from matplotlib.ticker import FuncFormatter
import os

# Carregar o CSV
df = pd.read_csv("sorted_consolidated_results.csv")

# Ajustar o estilo do seaborn
sns.set(style="whitegrid")

# Definir o diretório de salvamento (ajuste conforme necessário)
output_dir = "grafico_imagens"
os.makedirs(output_dir, exist_ok=True)  # Cria o diretório, se não existir

# Criar gráficos para cada linha do DataFrame
for index, row in df.iterrows():
    # Converter os valores de ms para ns (multiplicar por 1.000.000)
    mean_kmp_ns = row['mean_kmp'] * 1_000_000
    mean_brute_force_ns = row['mean_brute_force'] * 1_000_000
    p_value = row['p_value']  # Supondo que a coluna 'p_value' contenha o p-valor
   
    # Criando um DataFrame temporário com os dados
    temp_df = pd.DataFrame({
        'Algorithm': ['KMP', 'Brute Force'],
        'Mean Value': [mean_kmp_ns, mean_brute_force_ns]
    })
   
    # Criando o gráfico de barras com Seaborn
    plt.figure(figsize=(8, 6))  # Tamanho do gráfico
    ax = sns.barplot(x='Algorithm', y='Mean Value', data=temp_df, palette="muted", hue="Algorithm", legend=False)

    # Título e rótulos do gráfico
    ax.set_title(f"{row['data_file']}", fontsize=16)
    ax.set_xlabel('Algoritmos', fontsize=12)
   
    # Ajuste do título do eixo Y com base no valor de 'data_file'
    ax.set_ylabel('Tempo Médio (ns)', fontsize=12)  # Alterando de "ms" para "ns"

    # Se o arquivo for "exon1_sequence", formatar o eixo Y para garantir que será exibido como decimal
    if row['data_file'] == "exon1_sequence":
        # Usando FuncFormatter para garantir a exibição em formato decimal
        ax.yaxis.set_major_formatter(FuncFormatter(lambda x, _: f'{x}'))

    # Adicionando os valores acima das barras
    for i, value in enumerate(temp_df['Mean Value']):
        ax.text(i, value + 0.0000001, f"{value:.3f}", ha='center', va='bottom', fontsize=14)

    # Garantir que há 20% a mais de espaço na parte superior
    ax.set_ylim(0, max(temp_df['Mean Value']) * 1.2)  # Expande o limite superior do gráfico

    # Adicionar o p-valor como legenda no canto superior direito
    ax.text(1, 1.1, f"p-value: {p_value:.0f}", ha='right', va='top', fontsize=14,
            transform=ax.transAxes, color='black', bbox=dict(facecolor='white', edgecolor='None', boxstyle='round,pad=0.3'))

    # Melhorar o layout
    plt.tight_layout()

    # Definir o nome do arquivo de saída (garantir que o nome do arquivo seja seguro para o sistema de arquivos)
    safe_filename = f"grafico_{row['data_file']}.png"
    output_filename = os.path.join(output_dir, safe_filename)

    # Diagnóstico de salvamento
    print(f"Tentando salvar o gráfico como {output_filename}")

    # Salvar o gráfico
    try:
        plt.savefig(output_filename, dpi=300)
        print(f"Gráfico salvo como {output_filename}")
    except Exception as e:
        print(f"Erro ao salvar o gráfico: {e}")
   
    # Exibindo o gráfico
    plt.show()  # Certifique-se de chamar o plt.show() após plt.savefig()

    # Fechar o gráfico para liberar memória e evitar problemas de sobreposição
    plt.close()
