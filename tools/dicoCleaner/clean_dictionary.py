import os
import unicodedata

def clean_word(word):
    """
    Nettoie un mot : le met en minuscule, retire les accents et les tirets.
    """
    # 1. Convertir en minuscules
    cleaned = word.lower()

    # 2. Supprimer les accents (approche standard et robuste en Python)
    # unicodedata.normalize('NFD', s): décompose les caractères accentués en caractères de base + diacritiques
    # .encode('ascii', 'ignore'): encode en ASCII, ignorant les diacritiques (qui ne sont pas ASCII)
    # .decode('utf-8'): décode à nouveau en UTF-8 pour obtenir une chaîne Python propre
    cleaned = unicodedata.normalize('NFD', cleaned).encode('ascii', 'ignore').decode('utf-8')

    # 3. Supprimer les tirets
    cleaned = cleaned.replace('-', '')

    # Optionnel: Supprimer tout ce qui n'est PAS une lettre de l'alphabet ASCII (a-z)
    # Utile si d'autres caractères indésirables peuvent apparaître après le nettoyage des accents/tirets.
    # cleaned = ''.join(char for char in cleaned if 'a' <= char <= 'z')

    return cleaned

def main():
    input_file_path = os.path.join('..', '..', 'data', 'dico.txt')
    output_file_path = os.path.join('..', '..', 'data', 'dicoClean.txt')

    # Assurez-vous que le dossier 'data' existe
    output_dir = os.path.dirname(output_file_path)
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
        print(f"Dossier créé : {output_dir}")

    print("Début du nettoyage du dictionnaire...")
    processed_words = 0
    filtered_words = 0
    
    # Utilisation de 'with open' pour s'assurer que les fichiers sont correctement fermés
    try:
        with open(input_file_path, 'r', encoding='utf-8') as infile, \
             open(output_file_path, 'w', encoding='utf-8') as outfile:

            for line in infile:
                word = line.strip()  # Lire la ligne et supprimer les espaces blancs/sauts de ligne

                if not word:  # Ignorer les lignes vides
                    continue

                processed_words += 1

                cleaned_word = clean_word(word)

                # Filtrer les mots d'une seule lettre ou qui sont devenus vides
                if len(cleaned_word) > 1 and cleaned_word:
                    outfile.write(cleaned_word + '\n')
                else:
                    filtered_words += 1
                    # print(f"Filtré (trop court/vide après nettoyage) : '{word}' -> '{cleaned_word}'")

    except FileNotFoundError:
        print(f"Erreur : Le fichier d'entrée '{input_file_path}' est introuvable.")
        return
    except Exception as e:
        print(f"Une erreur inattendue est survenue : {e}")
        return

    print("Nettoyage terminé.")
    print(f"Mots traités : {processed_words}")
    print(f"Mots filtrés (1 lettre ou vide) : {filtered_words}")
    print(f"Fichier nettoyé enregistré sous : {output_file_path}")

if __name__ == "__main__":
    main()