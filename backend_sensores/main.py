
from datetime import datetime, timezone
from typing import Optional
import sqlite3

from fastapi import FastAPI, HTTPException
from pydantic import BaseModel, Field

app = FastAPI(title="API de Leituras dos Sensores")

BANCO = "sensores.db"


# Modelo dos dados recebidos
class Leitura(BaseModel):
    timestamp: datetime
    temperatura_c: Optional[float] = None
    umidade_pct: Optional[float] = None
    luminosidade: int = Field(ge=0, le=4095)
    presenca: int = Field(ge=0, le=1)
    probabilidade_vida: float = Field(ge=0, le=100)


# Criar tabela automaticamente
def inicializar_banco():
    with sqlite3.connect(BANCO) as conexao:
        conexao.execute("""
            CREATE TABLE IF NOT EXISTS leituras (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                timestamp TEXT NOT NULL,
                temperatura_c REAL,
                umidade_pct REAL,
                luminosidade INTEGER NOT NULL,
                presenca INTEGER NOT NULL,
                probabilidade_vida REAL NOT NULL
            )
        """)


def salvar_leitura(dados: Leitura):
    with sqlite3.connect(BANCO) as conexao:
        cursor = conexao.execute("""
            INSERT INTO leituras (
                timestamp,
                temperatura_c,
                umidade_pct,
                luminosidade,
                presenca,
                probabilidade_vida
            )
            VALUES (?, ?, ?, ?, ?, ?)
        """, (
            dados.timestamp.isoformat(),
            dados.temperatura_c,
            dados.umidade_pct,
            dados.luminosidade,
            dados.presenca,
            dados.probabilidade_vida
        ))

        return cursor.lastrowid


inicializar_banco()


# POST: receber e armazenar uma leitura
@app.post("/leituras", status_code=201)
def criar_leitura(dados: Leitura):
    try:
        leitura_id = salvar_leitura(dados)

        return {
            "mensagem": "Leitura armazenada com sucesso",
            "id": leitura_id
        }

    except sqlite3.Error as erro:
        raise HTTPException(
            status_code=500,
            detail=f"Erro ao salvar no banco: {erro}"
        )


# GET: consultar as últimas 100 leituras
@app.get("/leituras")
def consultar_leituras():
    try:
        with sqlite3.connect(BANCO) as conexao:
            conexao.row_factory = sqlite3.Row

            linhas = conexao.execute("""
                SELECT *
                FROM leituras
                ORDER BY id DESC
                LIMIT 100
            """).fetchall()

            return [dict(linha) for linha in linhas]

    except sqlite3.Error:
        raise HTTPException(
            status_code=500,
            detail="Erro ao consultar o banco de dados"
        )


@app.get("/")
def inicio():
    return {"mensagem": "API de sensores funcionando"}
