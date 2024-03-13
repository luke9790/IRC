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
Road map:

- uselist aggiornata per ogni evento (FATTO)

- da fare mode nome canale (FATTO)

-topic da aggiustare messaggi di "errore"

-USER e NICK da aggiustare

-in generale mettere messaggi di risposta ai comandi

- NON FUNZIONANO PIU CORRETTAMENTE GLI OPERATORI DEI CANALI (IL SECONDO CHE ARRIVA DIVENTA OPERATORE, IL PRIMO NO)

Codici di Risposta Numerici:
001 RPL_WELCOME
002 RPL_YOURHOST
003 RPL_CREATED
004 RPL_MYINFO
005 RPL_ISUPPORT
200 RPL_TRACELINK
201 RPL_TRACECONNECTING
202 RPL_TRACEHANDSHAKE
203 RPL_TRACEUNKNOWN
204 RPL_TRACEOPERATOR
205 RPL_TRACEUSER
206 RPL_TRACESERVER
208 RPL_TRACENEWTYPE
211 RPL_STATSLINKINFO
212 RPL_STATSCOMMANDS
213 RPL_STATSCLINE
214 RPL_STATSNLINE
215 RPL_STATSILINE
216 RPL_STATSKLINE
217 RPL_STATSQLINE
218 RPL_STATSYLINE
219 RPL_ENDOFSTATS
221 RPL_UMODEIS
231 RPL_SERVICEINFO
232 RPL_ENDOFSERVICES
233 RPL_SERVICE
234 RPL_SERVLIST
235 RPL_SERVLISTEND
241 RPL_STATSLLINE
242 RPL_STATSUPTIME
243 RPL_STATSOLINE
244 RPL_STATSHLINE
250 RPL_STATSDLINE
251 RPL_LUSERCLIENT
252 RPL_LUSEROP
253 RPL_LUSERUNKNOWN
254 RPL_LUSERCHANNELS
255 RPL_LUSERME
256 RPL_ADMINME
257 RPL_ADMINLOC1
258 RPL_ADMINLOC2
259 RPL_ADMINEMAIL
261 RPL_TRACELOG
262 RPL_TRACEEND
263 RPL_TRYAGAIN
265 RPL_LOCALUSERS
266 RPL_GLOBALUSERS
301 RPL_AWAY
302 RPL_USERHOST
303 RPL_ISON
305 RPL_UNAWAY
306 RPL_NOWAWAY
311 RPL_WHOISUSER
312 RPL_WHOISSERVER
313 RPL_WHOISOPERATOR
314 RPL_WHOWASUSER
315 RPL_ENDOFWHO
317 RPL_WHOISIDLE
318 RPL_ENDOFWHOIS
319 RPL_WHOISCHANNELS
321 RPL_LISTSTART
322 RPL_LIST
323 RPL_LISTEND
324 RPL_CHANNELMODEIS
329 RPL_CREATIONTIME
331 RPL_NOTOPIC
332 RPL_TOPIC
333 RPL_TOPICWHOTIME
341 RPL_INVITING
342 RPL_SUMMONING
351 RPL_VERSION
352 RPL_WHOREPLY
353 RPL_NAMREPLY
361 RPL_KILLDONE
362 RPL_CLOSING
363 RPL_CLOSEEND
364 RPL_LINKS
365 RPL_ENDOFLINKS
366 RPL_ENDOFNAMES
367 RPL_BANLIST
368 RPL_ENDOFBANLIST
369 RPL_ENDOFWHOWAS
371 RPL_INFO
372 RPL_MOTD
373 RPL_INFOSTART
374 RPL_ENDOFINFO
375 RPL_MOTDSTART
376 RPL_ENDOFMOTD
381 RPL_YOUREOPER
382 RPL_REHASHING
383 RPL_YOURESERVICE
384 RPL_MYPORTIS
385 RPL_NOTOPERANYMORE
391 RPL_TIME
392 RPL_USERSSTART
393 RPL_USERS
394 RPL_ENDOFUSERS
395 RPL_NOUSERS
401 ERR_NOSUCHNICK
402 ERR_NOSUCHSERVER
403 ERR_NOSUCHCHANNEL
404 ERR_CANNOTSENDTOCHAN
405 ERR_TOOMANYCHANNELS
406 ERR_WASNOSUCHNICK
407 ERR_TOOMANYTARGETS
408 ERR_NOSUCHSERVICE
409 ERR_NOORIGIN
411 ERR_NORECIPIENT
412 ERR_NOTEXTTOSEND
413 ERR_NOTOPLEVEL
414 ERR_WILDTOPLEVEL
415 ERR_BADMASK
421 ERR_UNKNOWNCOMMAND
422 ERR_NOMOTD
423 ERR_NOADMININFO
424 ERR_FILEERROR
431 ERR_NONICKNAMEGIVEN
432 ERR_ERRONEUSNICKNAME
433 ERR_NICKNAMEINUSE

Codici di Risposta Numerici:
001 - RPL_WELCOME
Questo codice viene inviato dal server per dare il benvenuto al client appena connesso.

002 - RPL_YOURHOST
Fornisce informazioni sull'host del server IRC al client.

003 - RPL_CREATED
Indica la data di creazione del server IRC.

004 - RPL_MYINFO
Fornisce informazioni sul server, come versione, patch, modi supportati, ecc.

005 - RPL_ISUPPORT
Invia una serie di opzioni supportate dal server, come estensioni del protocollo, modalità di canale, ecc.

200-206 - Trace e Trace Reply
Questi codici sono utilizzati per tracciare la route che il messaggio IRC sta seguendo.

208 - RPL_TRACENEWTYPE
Indica un nuovo tipo di connessione IRC.

211 - 259 - Statistiche
Questi codici forniscono informazioni statistiche sul server, come numero di utenti, operatori, link, ecc.

301 - 303 - Utente inattivo, utente host e lista utenti
Fornisce informazioni su utenti specifici, come stato "Away", host, e lista degli utenti online.

311 - 319 - Informazioni utente e canale
Fornisce informazioni dettagliate sugli utenti e sui canali.

321 - 323 - Elenco canali
Fornisce un elenco di canali disponibili sul server IRC.

324 - RPL_CHANNELMODEIS
Invia la modalità corrente di un canale IRC.

331 - RPL_NOTOPIC
Indica che il canale specificato non ha un argomento impostato.

332 - RPL_TOPIC
Invia l'argomento corrente di un canale IRC.

333 - RPL_TOPICWHOTIME
Fornisce informazioni sull'utente che ha impostato l'argomento e il timestamp.

341 - RPL_INVITING
Indica che l'utente è stato invitato con successo in un canale.

342 - RPL_SUMMONING
Indica che l'utente è stato chiamato con successo.

351 - RPL_VERSION
Invia informazioni sulla versione del server IRC.

352 - RPL_WHOREPLY
Fornisce informazioni sugli utenti attualmente presenti sul server.

353 - RPL_NAMREPLY
Fornisce l'elenco degli utenti nel canale specificato.

361 - RPL_KILLDONE
Indica che un kill di un utente è stato completato con successo.

362 - RPL_CLOSING
Indica che la connessione al server è stata chiusa.

363 - RPL_CLOSEEND
Indica che la fase di chiusura della connessione è stata completata.

364 - RPL_LINKS
Fornisce un elenco dei server collegati.

365 - RPL_ENDOFLINKS
Indica la fine della lista dei server collegati.

366 - RPL_ENDOFNAMES
Indica la fine dell'elenco degli utenti in un canale.

371 - 374 - MOTD (Message of the Day)
Questi codici vengono utilizzati per inviare il messaggio del giorno del server IRC.

375 - RPL_MOTDSTART
Indica l'inizio del messaggio del giorno del server.

376 - RPL_ENDOFMOTD
Indica la fine del messaggio del giorno del server.

381 - RPL_YOUREOPER
Indica che l'utente è stato promosso a operatore (IRCop).

382 - RPL_REHASHING
Indica che il server sta ri-caricando i file di configurazione.

383 - RPL_YOURESERVICE
Indica che l'utente è un servizio IRC.

384 - RPL_MYPORTIS
Indica la porta del client.

385 - RPL_NOTOPERANYMORE
Indica che l'utente non è più un operatore.

391 - RPL_TIME
Invia l'ora corrente del server IRC.

392 - 395 - Elenco utenti e fine
Fornisce un elenco degli utenti online e indica la fine dell'elenco.

401 - 415 - Errori comuni
Questi codici indicano errori comuni che possono verificarsi durante l'interazione IRC.
