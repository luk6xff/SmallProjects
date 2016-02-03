
import com.app.CircularBuffer;

import org.junit.Test;

import static org.junit.Assert.assertEquals;

/**
 * Created by igbt6 on 28.10.2015.
 */
public class CircularBufferUnitTest {



   // @Test
    public void checkCorrectInsertRemove() throws Exception {
        System.out.println("\n-----------------checkCorrectBehaviour------------------------");
        CircularBuffer cBuf = new CircularBuffer(3);
        byte[] data = new byte[]{(byte)0x01,0x0F,(byte)0x08,(byte)0x09,0x10,0x0A,0x0B};
        System.out.println("-------INSERTING-----------");
        for(int i=0;i<7;i++) {
            if(!cBuf.insert(data[i])){
                printBufeerData(cBuf);
            }
            else{

            }
            System.out.println("----------------------------------");
        }

        System.out.println("-------REMOVING-----------");
        for(int i=0;i<7;i++) {
            Byte b= cBuf.remove();
            if(b==null){
                printBufeerData(cBuf);
            }
            else{
                System.out.println("removed: "+String.valueOf(b));
            }
            System.out.println("----------------------------------");
        }


        assertEquals(1, 1); //dummy call
    }


    @Test
    public void checkCorrectPeeking() throws Exception {
        System.out.println("\n-----------------checkCorrectPeeking------------------------");
        CircularBuffer cBuf = new CircularBuffer(6);
        byte[] data = new byte[]{(byte)0x01,0x0F,(byte)0x08,(byte)0x09,0x10,0x0A,0x0B};
        System.out.println("-------INSERTING-----------");
        for(int i=0;i<4;i++) {
            cBuf.insert(data[i]);
        }
        assertEquals("01 0F 08 09 ", convertByteArrayToString(cBuf.peekBuf()));
        System.out.println("----------------------------------");

        System.out.println("-------REMOVING-----------");
        for(int i=0;i<2;i++) {
            cBuf.remove();
        }
        assertEquals("08 09 ", convertByteArrayToString(cBuf.peekBuf()));

        System.out.println("----------------------------------");
        System.out.println("-------INSERTING_2-----------");
        for(int i=0;i<4;i++) {
            cBuf.insert(data[i]);
        }
        assertEquals("08 09 01 0F 08 09 ", convertByteArrayToString(cBuf.peekBuf()));
        System.out.println("----------------------------------");


        System.out.println("-------PEEKING_WITH_LENGTH-----------");
        assertEquals("08 09 01 0F ", convertByteArrayToString(cBuf.peekBuf(4)));
        assertEquals("08 09 01 0F 08 ", convertByteArrayToString(cBuf.peekBuf(5)));
        assertEquals("08 09 01 0F 08 09 ", convertByteArrayToString(cBuf.peekBuf(6)));
        assertEquals(null, convertByteArrayToString(cBuf.peekBuf(7)));
        System.out.println("----------------------------------");
    }





    @Test
    public void checkRemoveBuffer() throws Exception {
        System.out.println("\n-----------------checkRemoveBuffer------------------------");
        CircularBuffer cBuf = new CircularBuffer(6);
        byte[] data = new byte[]{0x01,0x0F,0x08,0x09,0x10,0x0A,0x0B};

        System.out.println("-------INSERTING-----------");
        for(int i=0;i<4;i++) {
            cBuf.insert(data[i]);
        }
        assertEquals("01 0F 08 09 ", convertByteArrayToString(cBuf.peekBuf()));
        System.out.println("----------------------------------");

        System.out.println("-------REMOVING_BUFFER-----------");
        assertEquals("01 0F 08 ", convertByteArrayToString(cBuf.removeBuf(3)));
        assertEquals("09 ", convertByteArrayToString(cBuf.peekBuf()));
        System.out.println("----------------------------------");

        System.out.println("-------INSERTING_2-----------");
        for(int i=0;i<6;i++) {
            cBuf.insert(data[i]);
        }
        assertEquals("09 01 0F 08 09 10 ", convertByteArrayToString(cBuf.peekBuf()));
        System.out.println("----------------------------------");

        System.out.println("-------REMOVING_BUFFER_2-----------");
        System.out.println("-------IDX_OF_APPEARANCE----------: " + cBuf.getHead() + " " + cBuf.getIndexOfFirstAppearance((byte) 0x09));
        assertEquals("09 01 ", convertByteArrayToString(cBuf.removeBuf(2)));
        cBuf.removeBuf(2);
        assertEquals("09 10 ", convertByteArrayToString(cBuf.peekBuf()));
        cBuf.removeBuf(2);
        assertEquals(null, convertByteArrayToString(cBuf.peekBuf()));
    }


    @Test
    public void checkIndexOfFirstAppearance() throws Exception {
        System.out.println("\n-----------------checkIndexOfFirstAppearance------------------------");
        CircularBuffer cBuf = new CircularBuffer(6);
        byte[] data = new byte[]{0x01,0x0F,0x08,0x09,0x10,0x0A,0x0B};

        System.out.println("-------INSERTING-----------");
        for(int i=0;i<4;i++) {
            cBuf.insert(data[i]);
        }
        assertEquals(3, cBuf.getIndexOfFirstAppearance((byte) 0x09));
        System.out.println("----------------------------------");

        System.out.println("-------INSERTING_2-----------");
        for(int i=0;i<6;i++) {
            cBuf.insert(data[i]);
        }
        assertEquals("01 0F 08 09 01 0F ", convertByteArrayToString(cBuf.peekBuf()));
        System.out.println("----------------------------------");

        System.out.println("-------REMOVING-----------");
        cBuf.removeBuf(3);
        System.out.println(convertByteArrayToString(cBuf.peekBuf()) +" "+cBuf.getIndexOfFirstAppearance((byte) 0x0F));
        assertEquals(2, cBuf.getIndexOfFirstAppearance((byte) 0x0F));
        System.out.println("----------------------------------");


        System.out.println("-------INSERTING_3-----------");
        for(int i=0;i<6;i++) {
            cBuf.insert(data[i]);
        }
        assertEquals("09 01 0F 01 0F 08 ", convertByteArrayToString(cBuf.peekBuf()));
        assertEquals(5, cBuf.getIndexOfFirstAppearance((byte) 0x08));
        System.out.println("----------------------------------");

    }






    private void printBufeerData(CircularBuffer cBuf){
        System.out.println("mHead: "+String.valueOf(cBuf.getHead()));
        System.out.println("mTail: "+String.valueOf(cBuf.getTail()));
        System.out.println("mSize: "+String.valueOf(cBuf.getSize()));
        System.out.println("isFull: "+String.valueOf(cBuf.isFull()));
        System.out.println("isEmpty: "+String.valueOf(cBuf.isEmpty()));
        System.out.println("Peek_: "+String.valueOf(String.format("%X",cBuf.peek())));
        System.out.println("Buf_Copy: "+convertByteArrayToString(cBuf.peekBuf()));
    }
    
     private String convertByteArrayToString(Byte[] data) {
        if (data != null && data.length > 0) {
            final StringBuilder stringBuilder = new StringBuilder(data.length);
            for (Byte byteChar : data) {
                stringBuilder.append(String.format("%02X ", byteChar));
            }
            return stringBuilder.toString();
        } else {
            return null;
        }
    }

    private  String convertByteArrayToString(byte[] data) {
        if (data != null && data.length > 0) {
            final StringBuilder stringBuilder = new StringBuilder(data.length);
            for (byte byteChar : data) {
                stringBuilder.append(String.format("%02X ", byteChar));
            }
            return stringBuilder.toString();
        } else {
            return null;
        }
    }

}
