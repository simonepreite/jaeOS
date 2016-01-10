############################################################################

                        _                ____    _____          
                       (_)              / __ \  / ____|          
                        _   __ _   ___ | |  | || (___            
                       | | / _` | / _ \| |  | | \___ \           
                       | || (_| ||  __/| |__| | ____) |          
                       | | \__,_| \___| \____/ |_____/           
                      _/ |                                       
                     |__/                                        
              _____   ______            _____   __  __  ______ 
             |  __ \ |  ____|    /\    |  __ \ |  \/  ||  ____|
             | |__) || |__      /  \   | |  | || \  / || |__   
             |  _  / |  __|    / /\ \  | |  | || |\/| ||  __|  
             | | \ \ | |____  / ____ \ | |__| || |  | || |____ 
             |_|  \_\|______|/_/    \_\|_____/ |_|  |_||______|             
                                                    
                                                    
############################################################################
                                                   
#####                     DECOMPRIMERE IL PROGETTO                     #####

	- eseguire il seguente comando nella directory dove è contenuto
	  l'archivio:

	  $ cd /directory/archivio
	  $ tar -xvf jaeOS.tar.gz

############################################################################

#####                      COMPILAZIONE E PULIZIA                      #####

	- spostarsi della directory ottenuta dall'estrazione 
	  dell'archivio e digitare il comando:

	  $ make 

	  per ottenere la compilazione di tutti gli eseguibili necessari
	  che saranno:

	  jaeOS jaeOS.core.uarm jaeOS.stab.uarm

	- il make file offre la possibilità di eliminare i file creati
	  attraverso le opzioni:

	  & make clean  (che elimina il solo file eseguibile)
	  & make cleanall (che elimina anche gli altri file generati per uarm)

############################################################################

#####                            TEST P1TEST                           #####

	- per verificare il p1test sulla macchina aprimo un terminale ed
	  eseguiamo:

	  $ uarm 

	che avvierà la macchina spenta.

	clicchiamo sul pulsante delle impostazioni e nella sezione "Boot" 
	cerchiamo attraverso browse il file "jaeOS.core.uarm" (che si troverà
	nella cartella in cui abbiamo eseguito la compilazione).

	accendiamo l'emulatore attraverso il tasto power e apriamo un terminale
	da terminals.

	cliccare sul tasto "play" fino a esecuzione completata per verificare
	l'output.

############################################################################


                                                                                                  
                                                                                                  
                                                                           