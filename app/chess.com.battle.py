"""
WARNING: Dieses Script automatisiert das Spielen auf chess.com und kann gegen die Nutzungsbedingungen (TOS) von chess.com verstoßen. 
Nutze es nur zu Testzwecken und sei dir der Konsequenzen bewusst!

Voraussetzungen:
• Python-Pakete: selenium, python-chess, catmate (soweit verfügbar)
• Einen passenden WebDriver (z. B. chromedriver) im PATH
• Manuelle oder programmatische Authentifizierung bei chess.com

Dieses Script öffnet einen Browser, navigiert zu "chess.com/play/online", wartet auf den Beginn einer Partie (als Schwarz) 
und führt anschließend in jeder Runde folgende Schritte aus:
  1. Wartet, bis der Gegner seinen Zug gemacht hat.
  2. Liest den aktuellen Spielstand (FEN) und aktualisiert ein lokales python-chess Board.
  3. Ruft catmate.run(board=board, depth=6, save_data=False) auf, um den besten Zug zu ermitteln.
  4. Sendet den ermittelten Zug (z.B. "e2e4") an die chess.com-Oberfläche (durch Selenium-Steuerung) und aktualisiert lokal das Board.
  
ACHTUNG: Die genaue Implementierung zur Interaktion mit der chess.com-Webseite (DOM-Elemente, Klicks, etc.) ist stark
abhängig von deren aktueller Webseitenstruktur. Die folgenden Codeausschnitte sind als Ausgangspunkt zu verstehen.
"""

import time
import chess
from selenium import webdriver
from selenium.webdriver.common.by import By
# from selenium.webdriver.common.keys import Keys

# Importiere hier dein catmate Modul; es muss eine Funktion run(board, depth, save_data) bereitstellen,
# die einen Zug als UCI-String zurückgibt.
import catmate  # Stelle sicher, dass dieses Modul verfügbar ist

# Konfiguration
CHESS_URL = "https://www.chess.com/play/online"
POLL_INTERVAL = 2  # Sekunden, zwischen den Prüfungen auf neue Gegnerzüge

def login_and_start_game(driver):
    """
    Diese Funktion soll den Login-Prozess und das Starten einer Online-Partie automatisieren.
    Je nach Setup kann hier auch manuelles Login erfolgen (z.B. durch Userinteraktion im Browser).
    """
    driver.get(CHESS_URL)
    # Warten, bis die Seite geladen ist
    time.sleep(5)

    # OPTIONAL: Automatisiere den Login-Prozess, falls nötig.
    # Z.B.:
    # driver.find_element(By.ID, "username").send_keys("catmate@hannesscript.dev")
    # driver.find_element(By.ID, "password").send_keys("Catmate24!")
    # driver.find_element(By.ID, "login").click()

    # Warten, bis der Spielstart-Button vorhanden ist und starte eine Partie als Schwarz
    # Diese Logik muss an die aktuelle chess.com-Oberfläche angepasst werden.
    # Beispiel (Platzhalter):
    try:
        play_button = driver.find_element(By.XPATH, "//button[contains(text(),'Play')]")
        play_button.click()
        time.sleep(3)
        # Wähle ggf. Schwarz:
        black_button = driver.find_element(By.XPATH, "//button[contains(text(),'Play as Black')]")
        black_button.click()
    except Exception as e:
        print("Fehler beim Starten der Partie (vielleicht ist bereits ein Spiel aktiv?):", e)
    # Warte etwas, bis das Spiel vollständig geladen ist
    time.sleep(5)

def get_current_fen(driver):
    """
    Extrahiert die FEN-Zeichenkette über Computer Vision aus einem Screenshot der aktuellen Webseite.
    """
    try:
        driver.save_screenshot("board.png")
        # Hier würde die Computer-Vision-Logik implementiert werden
        fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
        return fen
    except Exception as e:
        print("Fehler beim Auslesen der FEN via Computer Vision:", e)
        return None

def send_move_to_chess(driver, move_uci):
    """
    Sendet den Zug (im UCI-Format, z. B. "e2e4") an die chess.com-Webseite.
    Dies passiert durch simulierte Mausklicks.
    """
    start_square = move_uci[:2]
    end_square = move_uci[2:]

    start_elem = driver.find_element(By.CSS_SELECTOR, f"div[data-square='{start_square}']")
    start_elem.click()
    time.sleep(0.3)

    end_elem = driver.find_element(By.CSS_SELECTOR, f"div[data-square='{end_square}']")
    end_elem.click()
    time.sleep(0.3)

def main():
    # Starte den WebDriver (hier: Chrome)
    driver = webdriver.Chrome()  # Stelle sicher, dass chromedriver im PATH ist
    driver.maximize_window()
    
    # Login und Partie starten
    login_and_start_game(driver)
    
    # Initialisiere ein lokales python-chess Board.
    # Falls möglich, hole die Start-FEN von chess.com; ansonsten nutze den Standard-Startzustand.
    starting_fen = chess.STARTING_FEN  # "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
    board = chess.Board(starting_fen)
    
    # Hauptschleife: Warte auf gegnerische Züge, aktualisiere das Board und führe deinen Zug aus.
    while not board.is_game_over():
        # Warte darauf, dass der Gegner einen Zug gemacht hat.
        # Hole die aktuelle FEN von chess.com
        current_fen = get_current_fen(driver)
        if current_fen:
            # Vergleiche mit der lokalen FEN; wenn sie unterschiedlich sind,
            # wurde ein neuer Zug gemacht. (Hier ist eine einfache Prüfung – in der Praxis 
            # müsste man die Unterschiede genau analysieren.)
            if current_fen != board.fen():
                print("Gegnerzug erkannt. Aktualisiere lokales Board.")
                board.set_fen(current_fen)
                
                # Berechne deinen Zug via catmate.run
                best_move_uci = catmate.run(board=board, depth=6, save_data=False)
                print("Bester Zug:", best_move_uci)
                
                # Aktualisiere das lokale Board
                try:
                    board.push_uci(best_move_uci)
                except Exception as e:
                    print("Fehler beim Aktualisieren des Boards:", e)
                
                # Sende den Zug an chess.com
                send_move_to_chess(driver, best_move_uci)
        
        time.sleep(POLL_INTERVAL)
    
    print("Partie beendet!")
    driver.quit()

if __name__ == "__main__":
    main()
