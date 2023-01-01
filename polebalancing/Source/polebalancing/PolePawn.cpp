#include "PolePawn.h"

// Sets default values
APolePawn::APolePawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	Cam = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Base = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base"));
	Pole = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pole"));

	Cam->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void APolePawn::BeginPlay()
{
	Super::BeginPlay();
	Open_Connection();

	CustomTimeDilation = 0.0000001f;												//author���ؿ���
	Pole->SetSimulatePhysics(false);												//author���ؿ���
	OnPause();
}

void APolePawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	Close_Connection();
}

// void APolePawn::OnPause() { }

// void APolePawn::OnResume() { }

// Called every frame
void APolePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (Input_ == 10) {
		UE_LOG(LogTemp, Warning, TEXT("#1_reset:input=10"));													//#1_
		Reset_Env();
		UE_LOG(LogTemp, Warning, TEXT("#1_reset:has"));
	}
	else if (Input_ == 11) {
		UE_LOG(LogTemp, Warning, TEXT("#1_x01: input=11"));														//#1_x01
		UE_LOG(LogTemp, Warning, TEXT("#2_x02: need+input_axis"))
	}
	else {
		// Apply Speed��CurrMotorSpeed���������ٶȣ����������������
		FVector currLocation = Base->GetComponentLocation();				//��������
		currLocation.X = currLocation.X + (CurrMotorSpeed * DeltaTime);		//x���˶��߼���baseλ��+base�ٶ�(Axis_Value������)
		Base->SetWorldLocation(currLocation);								//���� ���˶�
		CurrMotorSpeed *= 0.9f;
		UE_LOG(LogTemp, Warning, TEXT("#1_x02: movement"));														//#1_x02
	}

	// Conduct Connection - Gets Executed Once
	Conduct_Connection();

	if (Input_ != 10 && AppliedInput) {
		AppliedInput = false;										//�������ȴ����ٴο���
		//CustomTimeDilation = 0.0000001f;									//author���ؿ���
		//Pole->SetSimulatePhysics(false);									//author���ؿ���
		OnPause();


		FRotator rotator = Pole->GetRelativeRotation();
		float rotation = rotator.Pitch;														//ratation����			uint8 <- float		�쳣
		UE_LOG(LogTemp, Warning, TEXT("pitch: %f"), rotator.Pitch);	

		FVector BaseLocationVec = Base->GetComponentLocation();								//BaseLocationVec����	uint8 <- float		�쳣
		float BLocation = BaseLocationVec.X;
		UE_LOG(LogTemp, Warning, TEXT("Location: %f"), BaseLocationVec.X);

		int32 BytesSent = 0;

		//Tarry.send
		TArray<uint8> Bytes;																//Tarray����
		//Bytes.Add(rotation);																//ratation->Tarray[0]
		//ConnectionSocket->Send(Bytes.GetData(), Bytes.Num(), BytesSent);					//FSocket::Send(���ݣ����ݴ�С�����Ͷ���)

		//json.send
		TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);				//json����
		JsonObject->SetNumberField("angle", rotation);										//{"angle"= data,}_json
		JsonObject->SetNumberField("location", BLocation);									//{"angle"= data,"location"= data}_json
		FString OutputString;
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
		FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);						//fstring
										
		uint8 Outputjson = FCString::Atoi(*OutputString);									//FString->uint8	�β�->&->ʵ��
		//uint8 outbyte = 4096;
		//int32 buffer = 4096;
		//uint8 encode_json = StringToBytes(OutputString, &outbyte, buffer);				//Fstring->byte(uint8)

		TCHAR * pSendData = OutputString.GetCharArray().GetData();									//fstring->tchar->utf8
		int32 nDataLen = FCString::Strlen(pSendData);
		uint8 * dst = (uint8*)TCHAR_TO_UTF8(pSendData);

		//ConnectionSocket->Send(&Outputjson, Bytes.Num(), BytesSent);									//FSocket::Send(����uint8�����ݴ�С�����Ͷ���)
		ConnectionSocket->Send(dst, nDataLen, BytesSent);												//send(uint8-utf8)

		//key=0��ֹ�����ʾ
		GEngine->AddOnScreenDebugMessage(0, 40, FColor::Yellow, *OutputString);						//FSTring->json�ַ�������
		//GEngine->AddOnScreenDebugMessage(0, 40, FColor::Green, FString::FromInt(nDataLen));			//->jsonתsockt����
		
		
		UE_LOG(LogTemp, Warning, TEXT("#2_x01: send state"));														//#2_x01::py�ͷ��ˣ����Ӻ�
	}

	if (Input_ == 10) {
		Input_ = 11;
		UE_LOG(LogTemp, Warning, TEXT("#2_x02: end reset"));														//#2_x02::py�ͷ��ˣ����Ӻ�
	}

	if (HasInput) {
		HasInput = false;
		UE_LOG(LogTemp, Warning, TEXT("#2_x03: set recv[mod]"));													//#2_x03::py�ͷ��ˣ����Ӻ�
	}
}


void APolePawn::Conduct_Connection() {
	// Accept Connection
	if (WaitingForConnection) {
		TSharedRef<FInternetAddr> RemoteAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
		bool hasConnection = false;
		UE_LOG(LogTemp, Warning, TEXT("#3_layer_01:waiting"));															//#3_layer_01
		if (ListenSocket->HasPendingConnection(hasConnection) && hasConnection) {
			ConnectionSocket = ListenSocket->Accept(*RemoteAddress, TEXT("Connection"));						//FSocket::Accept�׽��ֵĵ�������
			WaitingForConnection = false;
			UE_LOG(LogTemp, Warning, TEXT("#3_incoming connection"));													//#3_layer_02

			// Start Recv Thread
			ClientConnectionFinishedFuture = Async(EAsyncExecution::LargeThreadPool, [&]() {
				UE_LOG(LogTemp, Warning, TEXT("#3_layer_03:connected"));												//#3_layer_03
				while (IsConnectionOpen) {
					uint32 size;
					TArray<uint8> ReceivedData;													//Tarray����

					if (ConnectionSocket->HasPendingData(size)) {
						ReceivedData.Init(0, 10);												//Tarray = [0,0,0,0,0,0,0,0,0,0]
						int i=0;
						//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("��ʼ��Tarray"));
						//UE_LOG(LogTemp, Warning, TEXT("Tarray"));
						while (i < ReceivedData.Num() -8)	{
								GEngine->AddOnScreenDebugMessage(0, 40, FColor::White, FString::FromInt(ReceivedData[i]));
								i++;
						}
																								//Tarray��֤= [0,0,0,0,0,0,0,0,0,0]
						int32 Read = 0;
						//write_Tarray��Tarr[0]���ݣ�������num����ָʾ���׽��ֶ�ȡ�˶����ֽڣ�
						//uint8* StrPtr = ReceivedData.GetData();					//m1	array1 == array2	ʵ�Σ�ֻ�ܵ�ȡ[0]
						//ReceivedData[1];											//m2	array[1]			�β�
						// .Last() / .Top()											�β� 
						uint8 *StrArr;												//m3	ָ��				success
						StrArr = &ReceivedData[3];									//m3	ָ���ַ->array[3]	->ʵ��[3]
						

						ConnectionSocket->Recv(ReceivedData.GetData(), ReceivedData.Num(), Read);			//ReceivedData.GetData()->m3	
						//UE_LOG(LogTemp, Warning, TEXT("TCP Recvdata: %f"), Input_);						//TCP���ݣ�����Ч����
						UE_LOG(LogTemp, Warning, TEXT("#3_layer_04:recv data"));										//#3_layer_04

						int a = 0;
						//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("tcp>>Tarray"));
						UE_LOG(LogTemp, Warning, TEXT("tcp>>Tarray"));
						while (a < ReceivedData.Num() -8) {
							GEngine->AddOnScreenDebugMessage(-1, 40, FColor::Yellow, FString::FromInt(ReceivedData[a]));
							a++;
						}																		//Tarray��֤: TCP���ܵ���Ϣ


						if (ReceivedData.Num() > 0)												//Tarray.num->��ȡ����
						{
							Input_ = ReceivedData[0];											//Tarray[0]read
							HasInput = true;
							//UE_LOG(LogTemp, Warning, TEXT("TCP�ƶ�����X: %f"), Input_)					//TCP����::�ƶ�����
							UE_LOG(LogTemp, Warning, TEXT("#3_layer_05:read"));											//#3_layer_05

							int b = 0;
							//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("getdata"));
							//UE_LOG(LogTemp, Warning, TEXT("getdata"));
							while (b < ReceivedData.Num()-8) {
								GEngine->AddOnScreenDebugMessage(-1, 40, FColor::Green, FString::FromInt(ReceivedData[b]));
								b++;
							}																	//Tarray��֤:���ݴ���input[0]


							// Handle Input
							if (HasInput) {
								CustomTimeDilation = 1.0f;										//author���ؿ���
								Pole->SetSimulatePhysics(true);									//author���ؿ���
								OnResume();
								AppliedInput = true;				 //actionӳ��ռ䣬n_action{1,2}->{-1,1}=����
								if (Input_ == 10) {												//input10==������
									// nothing!
									UE_LOG(LogTemp, Warning, TEXT("#3_layer_06:need+resetting"), Input_);				//#3_layer_06
								}
								else {															
									if (Input_ == 0) {										    //input 0==������
										Input_ = 0;
									}

								if (Input_ == 1)					//actionӳ��ռ䣬n_action{1,2}->{-1,1}=����
									Input_ = -1;
								else if (Input_ == 2)
									Input_ = 1;

									Move_XAxis(Input_);				//Move_XAxis:����ӳ��
								}
							}
						}
					}
				}
				});
		}
	}
}

void APolePawn::Reset_Env() {
	FVector currLocation = Base->GetComponentLocation();
	currLocation.X = 0;
	Base->SetWorldLocation(currLocation);					//�ָ�baseλ�ã�

	CurrMotorSpeed = 0;

	FRotator rotator = Pole->GetRelativeRotation();
	rotator.Pitch = 0.0f;
	rotator.Roll = 0.0f;
	rotator.Yaw = 0.0f;
	Pole->SetRelativeRotation(rotator);						//�ָ�pole��ת�� ue4�Լ���λ�ý���

	//
	FVector poleLocation = Pole->GetComponentLocation();
	poleLocation.X = 0;
	poleLocation.Z = 60;
	Pole->SetWorldLocation(poleLocation);
	//


	FVector angularVel = FVector();
	Pole->SetPhysicsAngularVelocity(angularVel);
	Pole->SetAllPhysicsLinearVelocity(angularVel);
}

// Called to bind functionality to input
void APolePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	InputComponent->BindAxis("X", this, &APolePawn::Move_XAxis);

	InputComponent->BindAction("OC", IE_Pressed, this, &APolePawn::Open_Connection);
	InputComponent->BindAction("CC", IE_Pressed, this, &APolePawn::Close_Connection);
}

void APolePawn::Move_XAxis(float AxisValue) {

	//Valueӳ������		base�ƶ�(1,-1,0��)>CurrMotorSpeed
	if (GEngine)
	{
		FString TheFloatStr = FString::SanitizeFloat(AxisValue);
		GEngine->AddOnScreenDebugMessage(0, 5.0f, FColor::Yellow, *TheFloatStr);						//Move_XAxis:����ӳ��+����ӳ��
	}
	//MotorPower;MaxSpeed������� �Զ����������
	CurrMotorSpeed += AxisValue * MotorPower; //Ĭ��MotorPower = 1����Ч��û��
	CurrMotorSpeed = FMath::Clamp(CurrMotorSpeed, -MaxSpeed, MaxSpeed);//��ЧCurrMotorSpeed=AxisValue������������ֵ��С��max��min����ʵ���ٶȴ�С������
}

void APolePawn::Open_Connection() {
	if (!IsConnectionOpen) {

		//�Զ�����begingame��Valueӳ������		TCPSOCKET
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("#5_Openning Connection"));		//#5_
		}

		UE_LOG(LogTemp, Warning, TEXT("#6_Openning Connection"));											//#6_
		IsConnectionOpen = true;
		WaitingForConnection = true;

		FIPv4Address IPAddress;
		FIPv4Address::Parse(FString("127.0.0.1"), IPAddress);
		FIPv4Endpoint Endpoint(IPAddress, (uint16)7794);

		ListenSocket = FTcpSocketBuilder(TEXT("TcpSocket")).AsReusable();									//δ֪��ӡ��ʽ

		ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
		ListenSocket->Bind(*SocketSubsystem->CreateInternetAddr(Endpoint.Address.Value, Endpoint.Port));
		ListenSocket->Listen(1);
		UE_LOG(LogTemp, Warning, TEXT("#8_Listening"));														//#8_

		//Valueӳ������
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Listening"));
		}
	}
}

void APolePawn::Close_Connection() {
	if (IsConnectionOpen) {
		UE_LOG(LogTemp, Warning, TEXT("#9_Closing Connection"));
		IsConnectionOpen = false;

		ListenSocket->Close();
	}
}