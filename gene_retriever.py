from Bio import Entrez, SeqIO

# Function to fetch the HTT gene reference sequence
def fetch_htt_gene_sequences(gene_id, email="your_email@example.com"):
    """
    Fetches the HTT gene reference, extracts exon 1 and the whole gene, 
    and stores them as strings.
    
    Parameters:
        gene_id (str): The gene ID for the HTT gene (e.g., RefSeq ID or Ensembl ID).
        email (str): Email address to use for Entrez API.
    
    Returns:
        tuple: Exon 1 sequence and whole gene sequence as strings.
    """
    # Set Entrez email (required for Entrez API usage)
    Entrez.email = email
    
    try:
        # Fetch the gene record
        handle = Entrez.efetch(db="nucleotide", id=gene_id, rettype="gb", retmode="text")
        record = SeqIO.read(handle, "genbank")
        handle.close()
        
        # Extract the whole gene sequence
        whole_gene_seq = str(record.seq)
        
        # Extract exon 1 sequence
        exon1_seq = ""
        for feature in record.features:
            if feature.type == "exon" and feature.qualifiers.get("number", [""])[0] == "1":
                exon1_seq = str(feature.location.extract(record).seq)
                break
        
        if not exon1_seq:
            return "Exon 1 not found in the provided gene reference.", whole_gene_seq
        
        return exon1_seq, whole_gene_seq
    
    except Exception as e:
        return f"Error fetching sequence: {e}", ""

# Function to save a sequence to a text file
def save_to_file(filename, content):
    """Save content to a text file."""
    with open(filename, 'w') as file:
        file.write(content)

# Example usage
gene_reference_id = "NG_009378.1"  # Replace with the correct HTT gene reference ID
exon1_sequence, whole_gene_sequence = fetch_htt_gene_sequences(gene_reference_id, email="your_email@example.com")

if exon1_sequence != "Exon 1 not found in the provided gene reference.":
    print("Exon 1 Sequence:", exon1_sequence)
    print("Whole Gene Sequence:", whole_gene_sequence)

    # Save sequences to text files
    save_to_file("exon1_sequence.txt", exon1_sequence)
    save_to_file("whole_gene_sequence.txt", whole_gene_sequence)
else:
    print(exon1_sequence)
