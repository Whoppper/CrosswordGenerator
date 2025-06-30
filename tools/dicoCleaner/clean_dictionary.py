import os
import unicodedata

def clean_word(word):
    cleaned = word.lower()
    cleaned = unicodedata.normalize('NFD', cleaned).encode('ascii', 'ignore').decode('utf-8')
    cleaned = cleaned.replace('-', '')
    return cleaned

def main():
    input_file_path = os.path.join('..', '..', 'data', 'input.txt')
    output_file_path = os.path.join('..', '..', 'data', 'dictionary.txt')
    output_dir = os.path.dirname(output_file_path)
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    processed_words = 0
    filtered_words = 0
    
    try:
        with open(input_file_path, 'r', encoding='utf-8') as infile, \
             open(output_file_path, 'w', encoding='utf-8') as outfile:

            for line in infile:
                word = line.strip()

                if not word:
                    continue

                processed_words += 1

                cleaned_word = clean_word(word)

                if len(cleaned_word) > 1 and cleaned_word:
                    outfile.write(cleaned_word + '\n')
                else:
                    filtered_words += 1

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