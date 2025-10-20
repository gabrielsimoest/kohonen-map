# 🧠 Kohonen Map – Wine Recognition Dataset

Este repositório contém uma **implementação de um Mapa de Kohonen (Self-Organizing Map - SOM)** aplicada ao conjunto de dados **Wine Recognition Data**, disponível no repositório da UCI Machine Learning Repository.

🔗 **Dataset original:** [Wine Recognition Data – UCI Machine Learning Repository](https://archive.ics.uci.edu/dataset/109/wine)

---

## 📘 Descrição do Projeto

O **Mapa de Kohonen** é uma rede neural não supervisionada utilizada para **redução de dimensionalidade** e **agrupamento de padrões**.  
Neste projeto, o SOM é treinado para organizar e visualizar os vinhos de acordo com suas características químicas.

O objetivo é identificar **relações entre as amostras** e **mapear a estrutura dos dados** em um espaço bidimensional, mantendo a topologia original.

---

## 🍷 Sobre o Dataset – Wine Recognition Data

### 📄 Informações Gerais

- **Título:** Wine Recognition Data  
- **Origem:**  
  - M. Forina et al., *PARVUS – An Extendible Package for Data Exploration, Classification and Correlation*, Institute of Pharmaceutical and Food Analysis and Technologies, Genoa, Italy.  
  - Stefan Aeberhard (1991), James Cook University of North Queensland.  
- **Atualização:** Setembro de 1998 (por C. Blake)  
- **Fonte:** [https://archive.ics.uci.edu/dataset/109/wine](https://archive.ics.uci.edu/dataset/109/wine)

---

### 📊 Contexto

Os dados são resultados de análises químicas de vinhos cultivados na mesma região da Itália, porém derivados de **três diferentes cultivares**.  
Cada amostra possui **13 atributos contínuos** correspondentes às concentrações de compostos químicos.

---

### 🧩 Atributos (13 variáveis)

1. Alcohol  
2. Malic acid  
3. Ash  
4. Alcalinity of ash  
5. Magnesium  
6. Total phenols  
7. Flavanoids  
8. Nonflavanoid phenols  
9. Proanthocyanins  
10. Color intensity  
11. Hue  
12. OD280/OD315 of diluted wines  
13. Proline  

---

### 🧪 Estrutura do Dataset

| Característica | Valor |
|----------------|--------|
| **Número de amostras** | 178 |
| **Número de atributos** | 13 |
| **Número de classes** | 3 |
| **Distribuição de classes** | Classe 1: 59 • Classe 2: 71 • Classe 3: 48 |
| **Valores ausentes** | Nenhum |
| **Tipo dos atributos** | Contínuos |

---

### 📈 Uso em pesquisas anteriores

O dataset foi amplamente utilizado para **comparação de classificadores** em ambientes de alta dimensionalidade:

- **RDA**: 100% de acurácia  
- **QDA**: 99.4%  
- **LDA**: 98.9%  
- **1-NN (z-transformado)**: 96.1%

Esses resultados foram obtidos utilizando a técnica *leave-one-out*.