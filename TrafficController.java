
public class TrafficController {
 int waitR = 0;
 int waitL = 0;
 int semaphore = 0;  
 int carIN = 0;
  
	public synchronized void enterLeft() {
		
		waitL++;

		if(carIN > 0||(semaphore <= -5 && waitR > 0)){
			try{																			
				wait();																	
			}																					
			catch (Exception ie) {}
		}
		carIN--;
		semaphore--;
		waitL--;
	}
  
	public synchronized void leaveLeft() {
    
		carIN--;
    
		if(carIN == 0){
			semaphore = 0;
			try{
					notify();
			}
			catch (Exception ie) {}
		}
	}
  
	public synchronized void enterRight() {

		waitR++;
    
		if(carIN < 0||(semaphore >= 5 && waitL > 0)){
			try{
				wait();
			}
			catch (Exception ie) {}
		}
		carIN++;
		semaphore++;
		waitR--;
	}
  
	public synchronized void leaveRight() {
		
		carIN++;
		
		if(carIN == 0){
			semaphore = 0;
			try{
					notify();
			}
			catch (Exception ie) {}
		}
	}
}