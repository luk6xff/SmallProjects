

/**
 * Created by igbt6 on 11.11.2015.
 */
public class CircularBuffer {

    private Byte[] mBuf;
    private int mMaxSize;
    private int mHead;
    private int mTail;
    private int mBufLen;


    public CircularBuffer(int size){
        mMaxSize=size;
        clear();
    }



    public void clear(){
        mBuf=new Byte[mMaxSize];
        mHead=0;
        mBufLen=0;
        mTail=0;
    }

    public boolean insert(byte b){
        if(!isFull()){
            mTail= (mTail+1)%mMaxSize;
            mBuf[mTail]=b;
            mBufLen++;
            return true;
        }
        else return false;
    }

    public Byte remove(){
        Byte retByte;
        if(!isEmpty()){
            mHead=(mHead+1)%mMaxSize;
            retByte= mBuf[mHead];
            mBufLen--;
        }
        else{
            retByte=null;
        }
        return retByte;
    }

    public Byte[] removeBuf(int length){
        Byte[] removeBuf=null;
        if(mBufLen>0&&length<=mBufLen&&length>0&&!isEmpty()) {
            removeBuf= new Byte[length];
            int copyIdx=0;
            for(int i=0;i<length;i++){
                mHead=(mHead+1)%mMaxSize;
                removeBuf[copyIdx] =mBuf[mHead];
                mBufLen--;
                copyIdx++;
            }
        }
        return removeBuf;
    }


    public Byte peek(){
        if(mBufLen>0) {
            return mBuf[(mHead+1)%mMaxSize];
        }
        return null;
    }

    public Byte[] peekBuf(){
        Byte[] bufCopy=null;
        if(mBufLen>0) {
            bufCopy= new Byte[mBufLen];
            int copyIdx=0;

            for(int i=0;i<mBufLen;i++){
                bufCopy[copyIdx] =mBuf[(mHead+1+i)%mMaxSize];
                copyIdx++;
            }
        }
        return bufCopy;
    }

    public Byte[] peekBuf(int length){
        Byte[] bufCopy=null;
        if(mBufLen>0&&length<=mBufLen&&length>0) {
            bufCopy= new Byte[length];
            int copyIdx=0;
            for(int i=0;i<length;i++){
                bufCopy[copyIdx] =mBuf[(mHead+1+i)%mMaxSize];
                copyIdx++;
            }
        }
        return bufCopy;
    }

    public Byte[] getWholeBuf(){
        return mBuf;
    }

    public int getSize(){
        return mBufLen;
    }

    public int getIndexOfFirstAppearance(byte value){
        for(int i=0;i<mBufLen;i++){
            if((mBuf[(mHead+1+i)%mMaxSize])==value){
                return (i);
            }
        }
        return -1;
    }

    public int getHead(){
        return mHead;
    }

    public int getTail(){
        return mTail;
    }

    public boolean isEmpty(){
        return 0==mBufLen;
    }

    public boolean isFull(){
        return mMaxSize==mBufLen;
    }


}
