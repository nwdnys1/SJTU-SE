import { orderWSURL } from "../services/requestService";

function OrderWS({ sid, receiver, receiverId, setConsultations }) {
  
  useEffect(() => {
    initWebSocket(sid, res);
    return () => {
      ws.current?.close();
    };
  }, []); // 空数组确保仅首次加载时运行

 

  
}

export default OrderWS;
