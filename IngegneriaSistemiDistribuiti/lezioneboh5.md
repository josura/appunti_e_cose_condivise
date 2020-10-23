remote authenticator per consentire l'identificazione in remoto di un utente del sistema con una identità ben definità.
quindi :
## non ridondanza
    i database degli utenti non deve essere ricopiato in un altro server ma essere mantenuto in locale nella stessa locazione(no duplicazione perché darebbe luogo ad inconsistenze).
## trasparenza dei permessi
    come utente non si deve sapere dove sono i permessi o fare altre azioni troppo specifiche per il controllo di autenticazione, quindi tutto trasparente.
## consistenza permessi
    gli utenti hanno un ruolo consistente che deve essere rispettato nella stessa azienda e tra gli stessi domini(o domini differenti)
## accountability
    si deve avere un modo per registrare le azioni e gli accessi degli utenti alle risorse in modo da sapere chi fa cosa
# soluzione
    - server specializzato per autenticazione e autorizzazione
    - punto di ingresso per ridirezionare e controllare le richieste
    - il server specializzato è un proxy che si interfaccia tra le risorse interne e gli utenti esterni, implementando autenticazione